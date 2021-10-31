#include "Viewer.h"

#include <trlevel/ILevel.h>
#include <trview.graphics/DeviceWindow.h>
#include <trview.graphics/RenderTargetStore.h>
#include <trview.graphics/Sprite.h>
#include <trview.graphics/ViewportStore.h>
#include <trview.input/WindowTester.h>
#include "Resources/ResourceHelper.h"
#include "Resources/resource.h"

#include <trview.common/Strings.h>

namespace trview
{
    namespace
    {
        const float _CAMERA_MOVEMENT_SPEED_MULTIPLIER = 23.0f;
    }

    Viewer::Viewer(const Window& window, const std::shared_ptr<graphics::IDevice>& device, std::unique_ptr<IViewerUI> ui, std::unique_ptr<IPicking> picking,
        std::unique_ptr<input::IMouse> mouse, const std::shared_ptr<IShortcuts>& shortcuts, const std::shared_ptr<IRoute> route, const graphics::ISprite::Source& sprite_source,
        std::unique_ptr<ICompass> compass, std::unique_ptr<IMeasure> measure, const graphics::IRenderTarget::SizeSource& render_target_source, const graphics::IDeviceWindow::Source& device_window_source,
        std::unique_ptr<ISectorHighlight> sector_highlight)
        : _shortcuts(shortcuts), _camera(window.size()), _free_camera(window.size()), _timer(default_time_source()), _keyboard(window),
        _mouse(std::move(mouse)), _window_resizer(window), _alternate_group_toggler(window),
        _menu_detector(window), _device(device), _route(route), _window(window), _ui(std::move(ui)), _picking(std::move(picking)), _compass(std::move(compass)), _measure(std::move(measure)),
        _render_target_source(render_target_source), _sector_highlight(std::move(sector_highlight))
    {
        apply_acceleration_settings();

        _scene_target = _render_target_source(static_cast<uint32_t>(window.size().width), static_cast<uint32_t>(window.size().height), graphics::IRenderTarget::DepthStencilMode::Enabled);
        _scene_sprite = sprite_source(window.size());
        _token_store += _free_camera.on_view_changed += [&]() { _scene_changed = true; };
        _token_store += _camera.on_view_changed += [&]() { _scene_changed = true; };

        _main_window = device_window_source(window);

        _token_store += _window_resizer.on_resize += [=]()
        {
            _main_window->resize();
            resize_elements();
        };

        _token_store += _alternate_group_toggler.on_alternate_group += [&](uint32_t group)
        {
            if (!_ui->is_input_active())
            {
                set_alternate_group(group, !alternate_group(group));
            }
        };

        initialise_input();

        _ui->initialise_input();
        _token_store += _ui->on_ui_changed += [&]() {_ui_changed = true; };
        _token_store += _ui->on_select_item += [&](uint32_t index)
        {
            if (_level && index < _level->items().size())
            {
                on_item_selected(_level->items()[index]);
            }
        };
        _token_store += _ui->on_select_room += [&](uint32_t room) { on_room_selected(room); };
        _token_store += _ui->on_highlight += [&](bool) { toggle_highlight(); };
        _token_store += _ui->on_show_hidden_geometry += [&](bool value) { set_show_hidden_geometry(value); };
        _token_store += _ui->on_show_water += [&](bool value) { set_show_water(value); };
        _token_store += _ui->on_show_wireframe += [&](bool value) { set_show_wireframe(value); };
        _token_store += _ui->on_show_triggers += [&](bool value) { set_show_triggers(value); };
        _token_store += _ui->on_show_bounding_boxes += [&](bool value) { set_show_bounding_boxes(value); };
        _token_store += _ui->on_flip += [&](bool value) { set_alternate_mode(value); };
        _token_store += _ui->on_alternate_group += [&](uint32_t group, bool value) { set_alternate_group(group, value); };
        _token_store += _ui->on_depth += [&](bool value) { if (_level) { _level->set_highlight_mode(ILevel::RoomHighlightMode::Neighbours, value); } };
        _token_store += _ui->on_depth_level_changed += [&](int32_t value) { if (_level) { _level->set_neighbour_depth(value); } };
        _token_store += _ui->on_camera_reset += [&]() { _camera.reset(); };
        _token_store += _ui->on_camera_mode += [&](CameraMode mode) { set_camera_mode(mode); };
        _token_store += _ui->on_camera_projection_mode += [&](ProjectionMode mode) { set_camera_projection_mode(mode); };
        _token_store += _ui->on_sector_hover += [&](const std::shared_ptr<ISector>& sector)
        {
            if (_level)
            {
                const auto room_info = _level->room_info(_level->selected_room());
                _sector_highlight->set_sector(sector,
                    DirectX::SimpleMath::Matrix::CreateTranslation(room_info.x / trlevel::Scale_X, 0, room_info.z / trlevel::Scale_Z));
                _scene_changed = true;
            }
        };
        _token_store += _ui->on_add_waypoint += [&]()
        {
            auto type = _context_pick.type == PickResult::Type::Entity ? IWaypoint::Type::Entity : _context_pick.type == PickResult::Type::Trigger ? IWaypoint::Type::Trigger : IWaypoint::Type::Position;
            if (_context_pick.triangle.normal.y != 0)
            {
                _context_pick.triangle.normal.x = 0;
                _context_pick.triangle.normal.z = 0;
                _context_pick.triangle.normal.Normalize();
            }
            on_waypoint_added(_context_pick.position, _context_pick.triangle.normal, room_from_pick(_context_pick), type, _context_pick.index);
        };
        _token_store += _ui->on_add_mid_waypoint += [&]()
        {
            auto type = _context_pick.type == PickResult::Type::Entity ? IWaypoint::Type::Entity : _context_pick.type == PickResult::Type::Trigger ? IWaypoint::Type::Trigger : IWaypoint::Type::Position;

            if (_context_pick.type == PickResult::Type::Room)
            {
                _context_pick.position = _context_pick.centroid;
            }
            else if (_context_pick.type == PickResult::Type::Entity)
            {
                _context_pick.position = _level->items()[_context_pick.index].position();
            }
            else if (_context_pick.type == PickResult::Type::Trigger)
            {
                _context_pick.position = _level->triggers()[_context_pick.index].lock()->position();
            }

            // Filter out non-wall normals - ceiling and floor normals should be vertical.
            if (_context_pick.triangle.normal.y != 0)
            {
                _context_pick.triangle.normal.x = 0;
                _context_pick.triangle.normal.z = 0;
                _context_pick.triangle.normal.Normalize();
            }

            on_waypoint_added(_context_pick.position, _context_pick.triangle.normal, room_from_pick(_context_pick), type, _context_pick.index);
        };
        _token_store += _ui->on_remove_waypoint += [&]() { on_waypoint_removed(_context_pick.index); };
        _token_store += _ui->on_hide += [&]()
        {
            if (_context_pick.type == PickResult::Type::Entity)
            {
                on_item_visibility(_level->items()[_context_pick.index], false);
            }
            else if (_context_pick.type == PickResult::Type::Trigger)
            {
                on_trigger_visibility(_level->triggers()[_context_pick.index], false);
            }
        };
        _token_store += _ui->on_orbit += [&]()
        {
            bool was_alternate_select = _was_alternate_select;
            on_room_selected(room_from_pick(_context_pick));
            if (_settings.auto_orbit && !was_alternate_select)
            {
                set_camera_mode(CameraMode::Orbit);
            }

            _target = _context_pick.position;

            auto stored_pick = _context_pick;
            stored_pick.override_centre = true;
            stored_pick.type = PickResult::Type::Room;
            add_recent_orbit(stored_pick);
        };
        _ui->on_settings += on_settings;
        _token_store += _ui->on_tool_selected += [&](auto tool) { _active_tool = tool; _measure->reset(); };
        _token_store += _ui->on_camera_position += [&](const auto& position)
        {
            if (_camera_mode == CameraMode::Orbit)
            {
                set_camera_mode(CameraMode::Free);
            }
            _free_camera.set_position(position);
        };
        _token_store += _ui->on_camera_rotation += [&](auto yaw, auto pitch)
        {
            current_camera().set_rotation_yaw(yaw);
            current_camera().set_rotation_pitch(pitch);
        };

        _token_store += _ui->on_command += [&](const auto& command)
        {
            lua_execute ( to_utf8 ( command ) );
        };

        _ui->set_settings(_settings);
        _ui->set_camera_mode(CameraMode::Orbit);

        _token_store += _measure->on_visible += [&](bool show) { _ui->set_show_measure(show); };
        _token_store += _measure->on_position += [&](auto pos) { _ui->set_measure_position(pos); };
        _token_store += _measure->on_distance += [&](float distance) { _ui->set_measure_distance(distance); };

        _token_store += _picking->pick_sources += [&](PickInfo, PickResult& result) { result.stop = !should_pick(); };
        _token_store += _picking->pick_sources += [&](PickInfo info, PickResult& result)
        {
            if (result.stop || _active_tool != Tool::None)
            {
                _compass_axis.reset();
                return;
            }

            Compass::Axis axis;
            if (_compass->pick(info.screen_position, info.screen_size, axis))
            {
                result.hit = true;
                result.stop = true;
                result.position = info.position + info.direction;
                result.type = PickResult::Type::Compass;
                result.index = static_cast<uint32_t>(axis);
                result.distance = 1.0f;
                _compass_axis = axis;
            }
            else
            {
                _compass_axis.reset();
            }
        };
        _token_store += _picking->pick_sources += [&](PickInfo info, PickResult& result)
        {
            if (result.stop || !_level)
            {
                return;
            }
            result = nearest_result(result, _level->pick(current_camera(), info.position, info.direction));
        };
        _token_store += _picking->pick_sources += [&](PickInfo info, PickResult& result)
        {
            if (result.stop)
            {
                return;
            }
            result = nearest_result(result, _route->pick(info.position, info.direction));
        };
        _token_store += _picking->pick_sources += [&](PickInfo, PickResult& result)
        {
            if (_active_tool == Tool::Measure && result.hit && !result.stop)
            {
                _measure->set(result.position);
                _scene_changed = true;
                if (_measure->measuring())
                {
                    result.text = _measure->distance();
                }
                else
                {
                    result.text = L"|....|";
                }
            }
        };

        _token_store += _picking->on_pick += [&](const PickInfo& pickInfo, const PickResult& result)
        {
            _current_pick = result;

            _ui->set_pick(pickInfo, result);

            // Highlight sectors in the minimap.
            if (_level)
            {
                std::optional<RoomInfo> info;
                if (result.hit)
                {
                    if (_current_pick.type == PickResult::Type::Room &&
                        _current_pick.index == _level->selected_room())
                    {
                        const auto room = _level->room(_current_pick.index).lock();
                        if (room)
                        {
                            info = room->info();
                        }
                    }
                    else if (_current_pick.type == PickResult::Type::Trigger)
                    {
                        auto trigger = _level->triggers()[_current_pick.index].lock();
                        if (trigger && trigger->room() == _level->selected_room())
                        {
                            if (const auto room = _level->room(trigger->room()).lock())
                            {
                                info = room->info();
                            }
                        }
                    }
                }

                if (!info.has_value())
                {
                    _ui->clear_minimap_highlight();
                    return;
                }

                auto x = _current_pick.position.x - (info.value().x / trlevel::Scale_X);
                auto z = _current_pick.position.z - (info.value().z / trlevel::Scale_Z);
                _ui->set_minimap_highlight(static_cast<uint16_t>(x), static_cast<uint16_t>(z));
            }
        };

        _token_store += _menu_detector.on_menu_toggled += [&](bool)
        {
            _timer.reset();
            _camera_input.reset();
        };

        register_lua();
    }

    void Viewer::initialise_input()
    {
        _token_store += _keyboard.on_key_up += [&](auto key, bool, bool) 
        {
            if (!_ui->is_input_active())
            {
                _camera_input.key_up(key);
            }
        };

        auto add_shortcut = [&](bool control, uint16_t key, std::function<void ()> fn)
        {
            _token_store += _shortcuts->add_shortcut(control, key) += [&, fn]()
            {
                if (!_ui->is_input_active()) { fn(); }
            };
        };

        add_shortcut(false, 'P', [&]() { toggle_alternate_mode(); });
        add_shortcut(false, 'M', [&]()
        {
            _active_tool = Tool::Measure;
            _measure->reset();
            _scene_changed = true;
        });
        add_shortcut(false, 'T', [&]() { toggle_show_triggers(); });
        add_shortcut(false, 'G', [&]() { toggle_show_hidden_geometry(); });
        add_shortcut(false, VK_OEM_MINUS, [&]() { select_previous_orbit(); });
        add_shortcut(false, VK_OEM_PLUS, [&]() { select_next_orbit(); });
        add_shortcut(false, VK_F1, [&]() { _ui->toggle_settings_visibility(); });
        add_shortcut(false, 'H', [&]() { toggle_highlight(); });
        add_shortcut(false, VK_INSERT, [&]()
        {
            // Reset the camera to defaults.
            _camera.set_rotation_yaw(0.f);
            _camera.set_rotation_pitch(-0.78539f);
            _camera.set_zoom(8.f);
        });

        _token_store += _keyboard.on_key_down += [&](uint16_t key, bool control, bool)
        {
            if (!_ui->is_input_active() && !_ui->show_context_menu())
            {
                _camera_input.key_down(key, control);
            }
            else if (_ui->show_context_menu() && key == VK_ESCAPE)
            {
                _ui->set_show_context_menu(false);
            }
        };

        setup_camera_input();

        using namespace input;

        _token_store += _mouse->mouse_click += [&](Mouse::Button button)
        {
            if (button == Mouse::Button::Left)
            {
                if (!_ui->is_cursor_over())
                {
                    if (_ui->show_context_menu())
                    {
                        _ui->set_show_context_menu(false);
                        return;
                    }

                    _ui->set_show_context_menu(false);

                    if (_compass_axis.has_value())
                    {
                        align_camera_to_axis(current_camera(), _compass_axis.value());
                        _compass_axis.reset();
                    }
                    else if (_current_pick.hit)
                    {
                        if (_active_tool == Tool::Measure)
                        {
                            if (_measure->add(_current_pick.position))
                            {
                                _active_tool = Tool::None;
                            }
                        }
                        else
                        {
                            select_pick(_current_pick);
                            
                            if (_settings.auto_orbit)
                            {
                                set_camera_mode(CameraMode::Orbit);
                                auto stored_pick = _current_pick;
                                stored_pick.position = _target;
                                add_recent_orbit(stored_pick);
                            }
                        }
                    }
                }
                else if (std::shared_ptr<ISector> sector = _ui->current_minimap_sector())
                {
                    // Select the trigger (if it is a trigger).
                    const auto triggers = _level->triggers();
                    auto trigger = std::find_if(triggers.begin(), triggers.end(),
                        [&](auto t)
                    {
                        const auto t_ptr = t.lock();
                        return t_ptr->room() == sector->room() && t_ptr->sector_id() == sector->id();
                    });

                    if (trigger == triggers.end() || (GetAsyncKeyState(VK_CONTROL) & 0x8000))
                    {
                        if (sector->flags() & SectorFlag::Portal)
                        {
                            on_room_selected(sector->portal());
                        }
                        else if (!_settings.invert_map_controls && (sector->flags() & SectorFlag::RoomBelow))
                        {
                            on_room_selected(sector->room_below());
                        }
                        else if (_settings.invert_map_controls && (sector->flags() & SectorFlag::RoomAbove))
                        {
                            on_room_selected(sector->room_above());
                        }
                    }
                    else
                    {
                        on_trigger_selected(*trigger);
                    }
                }
            }
            else if (button == Mouse::Button::Right)
            {
                _ui->set_show_context_menu(false);

                if (auto sector = _ui->current_minimap_sector())
                {
                    if (!_settings.invert_map_controls && (sector->flags() & SectorFlag::RoomAbove))
                    {
                        on_room_selected(sector->room_above());
                    }
                    else if (_settings.invert_map_controls && (sector->flags() & SectorFlag::RoomBelow))
                    {
                        on_room_selected(sector->room_below());
                    }
                }
            }
        };

        _token_store += _mouse->mouse_click += [&](auto button)
        {
            if (button == input::IMouse::Button::Right && _current_pick.hit && _current_pick.type != PickResult::Type::Compass)
            {
                _context_pick = _current_pick;
                _ui->set_show_context_menu(true);
                _camera_input.reset(true);
                _ui->set_remove_waypoint_enabled(_current_pick.type == PickResult::Type::Waypoint);
                _ui->set_hide_enabled(_current_pick.type == PickResult::Type::Entity || _current_pick.type == PickResult::Type::Trigger);
                _ui->set_mid_waypoint_enabled(_current_pick.type == PickResult::Type::Room && _current_pick.triangle.normal.y < 0);
            }
        };
    }

    void Viewer::update_camera()
    {
        if (_camera_mode == CameraMode::Free || _camera_mode == CameraMode::Axis)
        {
            const float Speed = std::max(0.01f, _settings.camera_movement_speed) * _CAMERA_MOVEMENT_SPEED_MULTIPLIER;
            _free_camera.move(_camera_input.movement() * Speed, _timer.elapsed());

            if (_level)
            {
                _level->on_camera_moved();
            }
        }

        current_camera().update(_timer.elapsed());
    }

    void Viewer::open(ILevel* level)
    {
        _level = level;

        _token_store += _level->on_alternate_mode_selected += [&](bool enabled) { set_alternate_mode(enabled); };
        _token_store += _level->on_alternate_group_selected += [&](uint16_t group, bool enabled) { set_alternate_group(group, enabled); };
        _token_store += _level->on_level_changed += [&]() { _scene_changed = true; };

        _level->set_show_triggers(_ui->show_triggers());
        _level->set_show_hidden_geometry(_ui->show_hidden_geometry());
        _level->set_show_water(_ui->show_water());
        _level->set_show_wireframe(_ui->show_wireframe()); 
        _level->set_show_bounding_boxes(_ui->show_bounding_boxes());

        // Set up the views.
        auto rooms = _level->room_info();
        _camera.reset();

        // Reset UI buttons
        _ui->set_max_rooms(static_cast<uint32_t>(rooms.size()));
        _ui->set_highlight(false);
        _ui->set_use_alternate_groups(_level->version() >= trlevel::LevelVersion::Tomb4);
        _ui->set_alternate_groups(_level->alternate_groups());
        _ui->set_flip(false);
        _ui->set_flip_enabled(_level->any_alternates());

        Item lara;
        if (_settings.go_to_lara && find_item_by_type_id(*_level, 0u, lara))
        {
            on_item_selected(lara);
        }
        else
        {
            on_room_selected(0);
        }

        _ui->set_depth_enabled(false);
        _ui->set_depth_level(1);

        // Strip the last part of the path away.
        const auto filename = _level->filename();
        auto last_index = std::min(filename.find_last_of('\\'), filename.find_last_of('/'));
        auto name = last_index == filename.npos ? filename : filename.substr(std::min(last_index + 1, filename.size()));
        _ui->set_level(name, _level->version());
        _window.set_title("trview - " + name);
        _measure->reset();

        _recent_orbits.clear();
        _recent_orbit_index = 0u;

        _scene_changed = true;
    }

    void Viewer::render()
    {
        _timer.update();
        update_camera();

        if (_mouse_changed || _scene_changed)
        {
            _picking->pick(_window, current_camera());
            _mouse_changed = false;
        }

        if (_scene_changed || _ui_changed)
        {
            _device->begin();
            _main_window->begin();
            _main_window->clear(Colour(_settings.background_colour));

            if (_scene_changed)
            {
                _scene_target->clear(Colour::Transparent);

                graphics::RenderTargetStore rs_store(_device->context());
                graphics::ViewportStore vp_store(_device->context());
                _scene_target->apply();

                render_scene();
                _scene_changed = false;
            }

            _scene_sprite->render(_scene_target->texture(), 0, 0, _window.size().width, _window.size().height);
            _ui->set_camera_position(current_camera().position());
            _ui->set_camera_rotation(current_camera().rotation_yaw(), current_camera().rotation_pitch());

            _ui->render();
            _ui_changed = false;

            _main_window->present(_settings.vsync);
        }

        lua_fire_event ( LuaEvent::ON_RENDER );
    }

    bool Viewer::should_pick() const
    {
        return !(!_level || window_under_cursor() != _window || window_is_minimised(_window) || _ui->is_cursor_over() || cursor_outside_window(_window));
    }

    void Viewer::render_scene()
    {
        if (_level)
        {
            // Update the view matrix based on the room selected in the room window.
            if (_level->number_of_rooms() > 0)
            {
                _camera.set_target(_target);
            }
            
            const auto& camera = current_camera();

            _level->render(camera, _show_selection);
            _sector_highlight->render(camera, _level->texture_storage());

            _measure->render(camera, _level->texture_storage());

            if (_show_route)
            {
                _route->render(camera, _level->texture_storage());
            }

            _level->render_transparency(camera);
            _compass->render(camera, _level->texture_storage());
        }
    }

    const ICamera& Viewer::current_camera() const
    {
        if (_camera_mode == CameraMode::Orbit)
        {
            return _camera;
        }
        return _free_camera;
    }

    ICamera& Viewer::current_camera()
    {
        if (_camera_mode == CameraMode::Orbit)
        {
            return _camera;
        }
        return _free_camera;
    }

    void Viewer::set_camera_mode(CameraMode camera_mode)
    {
        if (_camera_mode == camera_mode) 
        {
            return;
        }

        if (camera_mode == CameraMode::Free || camera_mode == CameraMode::Axis)
        {
            _free_camera.set_alignment(camera_mode_to_alignment(camera_mode));
            if (_camera_mode == CameraMode::Orbit)
            {
                _free_camera.set_position(_camera.position());
                _free_camera.set_rotation_yaw(_camera.rotation_yaw());
                _free_camera.set_rotation_pitch(_camera.rotation_pitch());
            }
        }

        _camera_mode = camera_mode;
        _ui->set_camera_mode(camera_mode);
        _scene_changed = true;
    }

    void Viewer::set_camera_projection_mode(ProjectionMode projection_mode)
    {
        _free_camera.set_projection_mode(projection_mode);
        _camera.set_projection_mode(projection_mode);
        _ui->set_camera_projection_mode(projection_mode);
        _scene_changed = true;
    }

    void Viewer::toggle_highlight()
    {
        if (_level)
        {
            bool new_value = !_level->highlight_mode_enabled(Level::RoomHighlightMode::Highlight);
            _level->set_highlight_mode(Level::RoomHighlightMode::Highlight, new_value);
            _ui->set_highlight(new_value);
        }
    }

    void Viewer::select_room(uint32_t room_number)
    {
        if (!_level || room_number >= _level->number_of_rooms())
        {
            return;
        }

        const auto room = _level->room(room_number).lock();
        if (!room)
        {
            return;
        }

        _ui->set_selected_room(room);
        _was_alternate_select = false;
        _target = room->centre();
        _scene_changed = true;
        if (_settings.auto_orbit)
        {
            set_camera_mode(CameraMode::Orbit);
        }
    }

    void Viewer::select_item(const Item& item)
    {
        _target = item.position();
        if (_settings.auto_orbit)
        {
            set_camera_mode(CameraMode::Orbit);
        }
        _scene_changed = true;
    }

    void Viewer::select_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        auto trigger_ptr = trigger.lock();
        _target = trigger_ptr->position();
        if (_settings.auto_orbit)
        {
            set_camera_mode(CameraMode::Orbit);
        }
        _scene_changed = true;
    }

    void Viewer::select_waypoint(const IWaypoint& waypoint)
    {
        _target = waypoint.position();
        if (_settings.auto_orbit)
        {
            set_camera_mode(CameraMode::Orbit);
        }
        _scene_changed = true;
    }

    void Viewer::set_route(const std::shared_ptr<IRoute>& route)
    {
        _route = route;
        _scene_changed = true;
    }

    void Viewer::set_show_compass(bool value)
    {
        _compass->set_visible(value);
        _scene_changed = true;
    }

    void Viewer::set_show_minimap(bool value)
    {
        _ui->set_show_minimap(value);
        _ui_changed = true;
    }

    void Viewer::set_show_route(bool value)
    {
        _show_route = value;
        _scene_changed = true;
    }

    void Viewer::set_show_selection(bool value)
    {
        _show_selection = value;
        _scene_changed = true;
    }

    void Viewer::set_show_tools(bool value)
    {
        _measure->set_visible(value);
        _scene_changed = true;
    }

    void Viewer::set_show_tooltip(bool value)
    {
        _ui->set_show_tooltip(value);
        _ui_changed = true;
    }

    void Viewer::set_show_ui(bool value)
    {
        _ui->set_visible(value);
        _ui_changed = true;
    }

    bool Viewer::ui_input_active() const
    {
        return _ui->is_input_active();
    }

    void Viewer::set_alternate_mode(bool enabled)
    {
        if (_level)
        {
            _was_alternate_select = true;
            _level->set_alternate_mode(enabled);
            _ui->set_flip(enabled);
        }
    }

    void Viewer::toggle_alternate_mode()
    {
        if (_level && _level->any_alternates())
        {
            set_alternate_mode(!_level->alternate_mode());
        }
    }

    void Viewer::set_alternate_group(uint32_t group, bool enabled)
    {
        if (_level)
        {
            _was_alternate_select = true;
            _level->set_alternate_group(group, enabled);
            _ui->set_alternate_group(group, enabled);
        }
    }

    bool Viewer::alternate_group(uint32_t group) const
    {
        if (_level)
        {
            return _level->alternate_group(group);
        }
        return false;
    }

    void Viewer::resize_elements()
    {
        const auto size = _window.size();
        // Inform elements that need to know that the device has been resized.
        _camera.set_view_size(size);
        _free_camera.set_view_size(size);
        _ui->set_host_size(size);
        _scene_target = _render_target_source(static_cast<uint32_t>(size.width), static_cast<uint32_t>(size.height), graphics::RenderTarget::DepthStencilMode::Enabled);
        _scene_sprite->set_host_size(size);
        _scene_changed = true;
    }

    // Set up keyboard and mouse input for the camera.
    void Viewer::setup_camera_input()
    {
        using namespace input;

        _token_store += _mouse->mouse_down += [&](auto button) { _camera_input.mouse_down(button); };
        _token_store += _mouse->mouse_up += [&](auto button) { _camera_input.mouse_up(button); };
        _token_store += _mouse->mouse_move += [&](long x, long y) { _mouse_changed = true; _camera_input.mouse_move(x, y); };
        _token_store += _mouse->mouse_wheel += [&](int16_t scroll) 
        {
            if (window_under_cursor() == _window)
            {
                _ui->set_show_context_menu(false);
                _camera_input.mouse_scroll(scroll);
            }
        };

        _token_store += _camera_input.on_rotate += [&](float x, float y)
        {
            if (_ui->is_cursor_over())
            {
                return;
            }

            if (_ui->show_context_menu())
            {
                _ui->set_show_context_menu(false);
            }

            ICamera& camera = current_camera();
            const float low_sensitivity = 200.0f;
            const float high_sensitivity = 25.0f;
            const float sensitivity = low_sensitivity + (high_sensitivity - low_sensitivity) * _settings.camera_sensitivity;
            camera.set_rotation_yaw(camera.rotation_yaw() + x / sensitivity);
            camera.set_rotation_pitch(camera.rotation_pitch() - y / sensitivity);
            if (_level)
            {
                _level->on_camera_moved();
            }
        };

        _token_store += _camera_input.on_zoom += [&](float zoom)
        {
            if (_ui->is_cursor_over())
            {
                return;
            }

            if (_camera_mode == CameraMode::Orbit)
            {
                _camera.set_zoom(_camera.zoom() + zoom);
                if (_level)
                {
                    _level->on_camera_moved();
                }
            }
            else if (_free_camera.projection_mode() == ProjectionMode::Orthographic)
            {
                _free_camera.set_zoom(_free_camera.zoom() + zoom);
                if (_level)
                {
                    _level->on_camera_moved();
                }
            }
        };

        _token_store += _camera_input.on_pan += [&](bool vertical, float x, float y)
        {
            if (_ui->is_cursor_over() || _camera_mode != CameraMode::Orbit)
            {
                return;
            }

            if (_ui->show_context_menu())
            {
                _ui->set_show_context_menu(false);
            }

            ICamera& camera = current_camera();

            using namespace DirectX::SimpleMath;

            if (camera.projection_mode() == ProjectionMode::Perspective)
            {
                if (vertical)
                {
                    _target += 0.05f * Vector3::Up * y * (_settings.invert_vertical_pan ? -1.0f : 1.0f);
                }
                else
                {
                    // Rotate forward and right by the camera yaw...
                    const auto rotation = Matrix::CreateRotationY(camera.rotation_yaw());
                    const auto forward = Vector3::Transform(Vector3::Forward, rotation);
                    const auto right = Vector3::Transform(Vector3::Right, rotation);

                    // Add them on to the position.
                    const auto movement = 0.05f * (forward * -y + right * -x);
                    _target += movement;
                }
            }
            else
            {
                auto rotate = Matrix::CreateFromYawPitchRoll(camera.rotation_yaw(), camera.rotation_pitch(), 0);
                _target += 0.05f * Vector3::Transform(Vector3(-x, y * (_settings.invert_vertical_pan ? -1.0f : 1.0f), 0), rotate);
            }

            if (_level)
            {
                _level->on_camera_moved();
            }
            _scene_changed = true;
        };

        _token_store += _camera_input.on_mode_change += [&](CameraMode mode) { set_camera_mode(mode); };
    }

    void Viewer::set_show_triggers(bool show)
    {
        if (_level)
        {
            _level->set_show_triggers(show);
            _ui->set_show_triggers(show);
        }
    }

    void Viewer::toggle_show_triggers()
    {
        if (_level)
        {
            set_show_triggers(!_level->show_triggers());
        }
    }

    void Viewer::set_show_hidden_geometry(bool show)
    {
        if (_level)
        {
            _level->set_show_hidden_geometry(show);
            _ui->set_show_hidden_geometry(show);
        }
    }

    void Viewer::toggle_show_hidden_geometry()
    {
        if (_level)
        {
            set_show_hidden_geometry(!_level->show_hidden_geometry());
        }
    }

    void Viewer::set_show_water(bool show)
    {
        if (_level)
        {
            _level->set_show_water(show);
        }
    }

    void Viewer::set_show_wireframe(bool show)
    {
        if (_level)
        {
            _level->set_show_wireframe(show);
            _ui->set_show_wireframe(show);
        }
    }

    void Viewer::set_show_bounding_boxes(bool show)
    {
        if (_level)
        {
            _level->set_show_bounding_boxes(show);
            _ui->set_show_bounding_boxes(show);
        }
    }

    uint32_t Viewer::room_from_pick(const PickResult& pick) const
    {
        switch (pick.type)
        {
        case PickResult::Type::Room:
            return pick.index;
        case PickResult::Type::Entity:
            return _level->items()[pick.index].room();
        case PickResult::Type::Trigger:
        {
            const auto trigger = _level->triggers()[pick.index].lock();
            if (trigger)
            {
                return trigger->room();
            }
            break;
        }
        case PickResult::Type::Waypoint:
            return _route->waypoint(pick.index).room();
        }
        return _level->selected_room();
    }

    void Viewer::add_recent_orbit(const PickResult& pick)
    {
        if (!_recent_orbits.empty())
        {
            _recent_orbits.resize(_recent_orbit_index + 1);
        }
        _recent_orbits.push_back(pick);
        _recent_orbit_index = _recent_orbits.size() - 1;
    }

    void Viewer::select_previous_orbit()
    {
        if (_recent_orbit_index > 0)
        {
            --_recent_orbit_index;
        }

        if (_recent_orbit_index < _recent_orbits.size())
        {
            const auto& pick = _recent_orbits[_recent_orbit_index];
            select_pick(pick);
        }
    }

    void Viewer::select_next_orbit()
    {
        if (_recent_orbits.size() && _recent_orbit_index < _recent_orbits.size() - 1)
        {
            ++_recent_orbit_index;
            const auto& pick = _recent_orbits[_recent_orbit_index];
            select_pick(pick);
        }
    }

    void Viewer::select_pick(const PickResult& pick)
    {
        switch (pick.type)
        {
        case PickResult::Type::Room:
            on_room_selected(pick.index);
            if (pick.override_centre)
            {
                _target = pick.position;
            }
            break;
        case PickResult::Type::Entity:
            on_item_selected(_level->items()[pick.index]);
            break;
        case PickResult::Type::Trigger:
            on_trigger_selected(_level->triggers()[pick.index]);
            break;
        case PickResult::Type::Waypoint:
            on_waypoint_selected(pick.index);
            break;
        }
    }

    void Viewer::apply_acceleration_settings()
    {
        _free_camera.set_acceleration_settings(_settings.camera_acceleration, _settings.camera_acceleration_rate);
    }

    void Viewer::set_settings(const UserSettings& settings)
    {
        _settings = settings;
        _ui->set_settings(_settings);
        apply_acceleration_settings();
        _scene_changed = true;
    }

    CameraMode Viewer::camera_mode() const
    {
        return _camera_mode;
    }
}
