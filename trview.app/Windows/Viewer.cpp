#include "Viewer.h"
#include "Resources/resource.h"

#include <trlevel/ILevel.h>
#include <trview.common/Strings.h>
#include <trview.common/Messages/Message.h>

#include "../Elements/Flyby/IFlybyNode.h"
#include "../Messages/Messages.h"
#include "../Elements/SoundSource/ISoundSource.h"
#include "../Elements/ILevel.h"
#include "../Windows/IWindow.h"
#include "../Filters/Filters.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    IViewer::~IViewer()
    {
    }

    Viewer::Viewer(const Window& window, const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<IViewerUI>& ui, std::unique_ptr<IPicking> picking,
        std::unique_ptr<input::IMouse> mouse, const std::shared_ptr<IShortcuts>& shortcuts, const std::shared_ptr<IRoute> route, std::unique_ptr<ICompass> compass,
        std::unique_ptr<IMeasure> measure, const graphics::IDeviceWindow::Source& device_window_source, std::unique_ptr<ISectorHighlight> sector_highlight,
        const std::shared_ptr<IClipboard>& clipboard, const std::shared_ptr<ICamera>& camera, const graphics::ISamplerState::Source& sampler_source,
        const std::weak_ptr<IMessageSystem>& messaging)
        : MessageHandler(window), _shortcuts(shortcuts), _timer(default_time_source()), _keyboard(window), _mouse(std::move(mouse)), _window_resizer(window),
        _alternate_group_toggler(window), _menu_detector(window), _device(device), _route(route), _ui(ui), _picking(std::move(picking)),
        _compass(std::move(compass)), _measure(std::move(measure)), _sector_highlight(std::move(sector_highlight)),
        _clipboard(clipboard), _camera(camera), _sampler_source(sampler_source), _messaging(messaging)
    {
        apply_camera_settings();

        _token_store += _camera->on_mode_changed += [&](auto mode)
            {
                _ui->set_camera_mode(mode);
                _camera_moved = true;
            };

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


        std::unordered_map<std::string, std::function<void(bool)>> toggles;
        toggles[Options::highlight] = [this](bool) { toggle_highlight(); };
        toggles[Options::geometry] = [this](bool value) { set_show_geometry(value); };
        toggles[Options::water] = [this](bool value) { set_show_water(value); };
        toggles[Options::wireframe] = [this](bool value) { set_show_wireframe(value); };
        toggles[Options::triggers] = [this](bool value) { set_show_triggers(value); };
        toggles[Options::show_bounding_boxes] = [this](bool value) { set_show_bounding_boxes(value); };
        toggles[Options::flip] = [this](bool value) { set_alternate_mode(value); };
        toggles[Options::depth_enabled] = [this](bool value) { if (auto level = _level.lock()) { level->set_highlight_mode(ILevel::RoomHighlightMode::Neighbours, value); } };
        toggles[Options::lights] = [this](bool value) { set_show_lights(value); };
        toggles[Options::items] = [this](bool value) { set_show_items(value); };
        toggles[Options::rooms] = [this](bool value) { set_show_rooms(value); };
        toggles[Options::camera_sinks] = [this](bool value) { set_show_camera_sinks(value); };
        toggles[Options::lighting] = [this](bool value) { set_show_lighting(value); };
        toggles[Options::animation] = [this](bool value) { set_show_animation(value); };
        toggles[Options::notes] = [](bool) {};
        toggles[Options::sound_sources] = [this](bool value) { set_show_sound_sources(value); };
        toggles[Options::ng_plus] = [this](bool value) { set_ng_plus(value); };

        const auto persist_toggle_value = [&](const std::string& name, bool value)
        {
            if (equals_any(name, Options::flip, Options::highlight, Options::depth_enabled))
            {
                return;
            }
            _settings.toggles[name] = value;
            messages::send_settings(_messaging, _settings);
        };

        std::unordered_map<std::string, std::function<void(int32_t)>> scalars;
        scalars[Options::depth] = [this](int32_t value) { if (auto level = _level.lock()) { level->set_neighbour_depth(value); } };

        _token_store += _ui->on_toggle_changed += [this, toggles, persist_toggle_value](const std::string& name, bool value)
        {
            auto toggle = toggles.find(name);
            if (toggle == toggles.end())
            {
                return;
            }
            toggle->second(value);
            persist_toggle_value(name, value);
        };
        _token_store += _ui->on_alternate_group += [&](uint32_t group, bool value) { set_alternate_group(group, value); };
        _token_store += _ui->on_scalar_changed += [this, scalars](const std::string& name, int32_t value)
        {
            auto scalar = scalars.find(name);
            if (scalar == scalars.end())
            {
                return;
            }
            scalar->second(value);
        };
        _token_store += _ui->on_camera_reset += [&]() { _camera->reset(); };
        _token_store += _ui->on_camera_mode += [&](auto mode) { set_camera_mode(mode); };
        _token_store += _ui->on_camera_projection_mode += [&](ProjectionMode mode) { set_camera_projection_mode(mode); };
        _token_store += _ui->on_add_waypoint += [&]()
        {
            auto type = _context_pick.type == PickResult::Type::Entity ? IWaypoint::Type::Entity : _context_pick.type == PickResult::Type::Trigger ? IWaypoint::Type::Trigger : IWaypoint::Type::Position;
            Vector3 normal = _context_pick.triangle.normal();
            if (type == IWaypoint::Type::Entity)
            {
                normal = Vector3::Down;
            }
            else if (normal.y != 0)
            {
                normal.x = 0;
                normal.z = 0;
                normal.Normalize();
            }

            uint32_t index = 0;
            switch (_context_pick.type)
            {
                case PickResult::Type::Room:
                {
                    if (const auto room = _context_pick.room.lock())
                    {
                        index = room->number();
                    }
                    break;
                }
                case PickResult::Type::Entity:
                {
                    if (const auto item = _context_pick.item.lock())
                    {
                        _context_pick.position = item->position();
                        index = item->number();
                    }
                    break;
                }
                case PickResult::Type::Trigger:
                {
                    if (const auto trigger = _context_pick.trigger.lock())
                    {
                        _context_pick.position = trigger->position();
                        index = trigger->number();
                    }
                    break;
                }
            }

            on_waypoint_added(_context_pick.position, normal, room_from_pick(_context_pick), type, index);
        };
        _token_store += _ui->on_add_mid_waypoint += [&]()
        {
            auto type = _context_pick.type == PickResult::Type::Entity ? IWaypoint::Type::Entity : _context_pick.type == PickResult::Type::Trigger ? IWaypoint::Type::Trigger : IWaypoint::Type::Position;

            uint32_t index = 0;
            if (_context_pick.type == PickResult::Type::Room)
            {
                if (const auto room = _context_pick.room.lock())
                {
                    _context_pick.position = _context_pick.centroid;
                    index = room->number();
                }
            }
            else if (_context_pick.type == PickResult::Type::Entity)
            {
                if (const auto item = _context_pick.item.lock())
                {
                    _context_pick.position = item->position();
                    index = item->number();
                }
            }
            else if (_context_pick.type == PickResult::Type::Trigger)
            {
                if (const auto trigger = _context_pick.trigger.lock())
                {
                    _context_pick.position = trigger->position();
                    index = trigger->number();
                }
            }

            // Filter out non-wall normals - ceiling and floor normals should be vertical.
            Vector3 normal = _context_pick.triangle.normal();
            if (normal.y != 0)
            {
                normal.x = 0;
                normal.z = 0;
                normal.Normalize();
            }

            on_waypoint_added(_context_pick.position, normal, room_from_pick(_context_pick), type, index);
        };
        _token_store += _ui->on_remove_waypoint += [&]() { on_waypoint_removed(_context_pick.waypoint_index); };
        _token_store += _ui->on_hide += [&]()
        {
            if (auto level = _level.lock())
            {
                if (_context_pick.type == PickResult::Type::Entity)
                {
                    if (auto item = _context_pick.item.lock())
                    {
                        item->set_visible(false);
                    }
                }
                else if (_context_pick.type == PickResult::Type::Trigger)
                {
                    if (auto trigger = _context_pick.trigger.lock())
                    {
                        trigger->set_visible(false);
                    }
                }
                else if (_context_pick.type == PickResult::Type::Light)
                {
                    if (auto light = _context_pick.light.lock())
                    {
                        light->set_visible(false);
                    }
                }
                else if (_context_pick.type == PickResult::Type::Room)
                {
                    if (auto room = _context_pick.room.lock())
                    {
                        room->set_visible(false);
                    }
                }
                else if (_context_pick.type == PickResult::Type::CameraSink)
                {
                    if (auto camera_sink = _context_pick.camera_sink.lock())
                    {
                        camera_sink->set_visible(false);
                    }
                }
                else if (_context_pick.type == PickResult::Type::StaticMesh)
                {
                    if (auto mesh = _context_pick.static_mesh.lock())
                    {
                        mesh->set_visible(false);
                    }
                }
                else if (_context_pick.type == PickResult::Type::SoundSource)
                {
                    if (auto sound_source = _context_pick.sound_source.lock())
                    {
                        sound_source->set_visible(false);
                    }
                }
            }
        };
        _token_store += _ui->on_orbit += [&]()
        {
            bool was_alternate_select = _was_alternate_select;
            messages::send_select_room(_messaging, room_from_pick(_context_pick));
            if (!was_alternate_select)
            {
                set_camera_mode(ICamera::Mode::Orbit);
            }

            set_target(_context_pick.position);

            auto stored_pick = _context_pick;
            stored_pick.override_centre = true;
            stored_pick.type = PickResult::Type::Room;
            add_recent_orbit(stored_pick);
        };
        _token_store += _ui->on_tool_selected += [&](auto tool) { _active_tool = tool; _measure->reset(); };
        _token_store += _ui->on_camera_position += [&](const auto& position)
        {
            if (camera_mode() == ICamera::Mode::Orbit)
            {
                set_camera_mode(ICamera::Mode::Free);
            }
            _camera->set_position(position);
        };
        _token_store += _ui->on_camera_rotation += [&](auto yaw, auto pitch)
        {
            _camera->set_rotation_yaw(yaw);
            _camera->set_rotation_pitch(pitch);
        };
        _token_store += _ui->on_copy += [&](auto type)
        {
            switch (type)
            {
                case IContextMenu::CopyType::Position:
                {
                    const auto pos = _context_pick.position * trlevel::Scale;
                    _clipboard->write(to_utf16(std::format("{:.0f}, {:.0f}, {:.0f}", pos.x, pos.y, pos.z)));
                    break;
                }
                case IContextMenu::CopyType::Number:
                {
                    auto get_number = [](auto&& e)
                        {
                            const auto e_ptr = e.lock();
                            return e_ptr ? e_ptr->number() : 0;
                        };

                    auto get_number_for_pick = [&](auto&& pick) -> uint32_t
                        {
                            switch (_context_pick.type)
                            {
                            case PickResult::Type::Waypoint:
                                return pick.waypoint_index;
                            case PickResult::Type::Room:
                                return get_number(pick.room);
                            case PickResult::Type::Entity:
                                return get_number(pick.item);
                            case PickResult::Type::Trigger:
                                return get_number(pick.trigger);
                            case PickResult::Type::StaticMesh:
                                return get_number(pick.static_mesh);
                            case PickResult::Type::Light:
                                return get_number(pick.light);
                            case PickResult::Type::CameraSink:
                                return get_number(pick.camera_sink);
                            case PickResult::Type::SoundSource:
                                return get_number(pick.sound_source);
                            default: // mesh, compass, TODO: scriptable 
                                return 0;
                            }
                        };

                    _clipboard->write(std::to_wstring(get_number_for_pick(_context_pick)));
                    break;
                }
            }
        };
        _ui->on_font += on_font;

        _token_store += _ui->on_filter_items_to_tile += [&](auto&& window_ptr)
            {
                if (!_context_pick.hit)
                {
                    return;
                }

                if (auto window = window_ptr.lock())
                {
                    const auto filter_to_sector = [&](auto&& room)
                        {
                            const auto info = room->info();
                            const auto sector_x = static_cast<int>(_context_pick.position.x - (info.x / trlevel::Scale_X));
                            const auto sector_z = static_cast<int>(_context_pick.position.z - (info.z / trlevel::Scale_Z));
                            window->receive_message(
                                Message{ .type = "item_filters", .data = std::make_shared<MessageData<std::vector<Filters::Filter>>>(
                                std::vector<Filters::Filter>
                                {
                                    {.key = "Room #", .compare = CompareOp::Equal, .value = std::to_string(room->number()), .op = Op::And },
                                    {.key = "X", .compare = CompareOp::Between, .value = std::to_string(info.x + sector_x * 1024), .value2 = std::to_string(info.x + (sector_x + 1) * 1024), .op = Op::And },
                                    {.key = "Z", .compare = CompareOp::Between, .value = std::to_string(info.z + sector_z * 1024), .value2 = std::to_string(info.z + (sector_z + 1) * 1024) }
                                }) });
                        };

                    const auto get_room = [&](auto&& ent)
                        {
                            if (const auto ent_ptr = ent.lock())
                            {
                                if (const auto room = ent_ptr->room().lock())
                                {
                                    filter_to_sector(room);
                                }
                            }
                        };

                    switch (_context_pick.type)
                    {
                        case PickResult::Type::Room:
                        {
                            if (const auto room = _context_pick.room.lock())
                            {
                                filter_to_sector(room);
                            }
                            break;
                        }
                        case PickResult::Type::Entity:
                            get_room(_context_pick.item);
                            break;
                        case PickResult::Type::Trigger:
                            get_room(_context_pick.trigger);
                            break;
                        case PickResult::Type::Light:
                            get_room(_context_pick.light);
                            break;
                        case PickResult::Type::CameraSink:
                            get_room(_context_pick.camera_sink);
                            break;
                    }
                }
            };

        _token_store += _ui->on_linear_filtering += [&](bool value)
            {
                const auto mode = value ? graphics::ISamplerState::FilterMode::Linear : graphics::ISamplerState::FilterMode::Point;
                _sampler_source(graphics::ISamplerState::AddressMode::Wrap)->set_filter_mode(mode);
                _sampler_source(graphics::ISamplerState::AddressMode::Clamp)->set_filter_mode(mode);
            };

        _ui->set_camera_mode(ICamera::Mode::Orbit);

        _token_store += _measure->on_visible += [&](bool show) { _ui->set_show_measure(show); };
        _token_store += _measure->on_position += [&](auto pos) { _ui->set_measure_position(pos); };
        _token_store += _measure->on_distance += [&](float distance) { _ui->set_measure_distance(distance); };

        _token_store += _picking->pick_sources += [&](PickInfo, PickResult& result) 
        {
            result.stop = !should_pick();
        };
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
                result.distance = 1.0f;
                result.text = axis_name(axis);
                _compass_axis = axis;
            }
            else
            {
                _compass_axis.reset();
            }
        };
        _token_store += _picking->pick_sources += [&](PickInfo info, PickResult& result)
        {
            const auto level = _level.lock();
            if (result.stop || !level)
            {
                return;
            }
            result = nearest_result(result, level->pick(*_camera, info.position, info.direction));
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
                if (_measure->measuring())
                {
                    result.text = _measure->distance();
                }
                else
                {
                    result.text = "|....|";
                }
            }
        };

        _token_store += _picking->on_pick += [&](PickInfo, PickResult result)
        {
            _current_pick = result;

            const auto level = _level.lock();
            if (level)
            {
                result.text = generate_pick_message(result, *level);
            }
            _ui->set_pick(result);

            if (result.stop)
            {
                return;
            }

            // Highlight sectors in the minimap.
            if (level)
            {
                std::optional<RoomInfo> info;
                if (result.hit)
                {
                    const auto selected_room = level->selected_room().lock();
                    if (_current_pick.type == PickResult::Type::Room)
                    {
                        const auto room = _current_pick.room.lock();
                        if (room && selected_room && room == selected_room)
                        {
                            info = selected_room->info();
                        }
                    }
                    else if (_current_pick.type == PickResult::Type::Trigger)
                    {
                        const auto trigger = _current_pick.trigger.lock();
                        if (trigger && selected_room && trigger->room().lock() == selected_room)
                        {
                            info = selected_room->info();
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

        _ui->set_route(_route);
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
        });
        add_shortcut(false, 'I', [&]() { toggle_show_items(); });
        add_shortcut(false, 'T', [&]() { toggle_show_triggers(); });
        add_shortcut(false, 'G', [&]() { toggle_show_geometry(); });
        add_shortcut(false, VK_OEM_MINUS, [&]() { select_previous_orbit(); });
        add_shortcut(false, VK_OEM_PLUS, [&]() { select_next_orbit(); });
        add_shortcut(false, VK_F1, [&]() { _ui->toggle_settings_visibility(); });
        add_shortcut(false, 'H', [&]() { toggle_highlight(); });
        add_shortcut(false, VK_INSERT, [&]()
        {
            // Reset the camera to defaults.
            _camera->set_rotation_yaw(0.f);
            _camera->set_rotation_pitch(-0.78539f);
            _camera->set_zoom(8.f);
        });
        add_shortcut(false, 'L', [&]() { toggle_show_lights(); });
        add_shortcut(true, 'H', [&]() { toggle_show_lighting(); });
        add_shortcut(false, 'K', [&]() { toggle_show_camera_sinks(); });

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

        _token_store += _mouse->mouse_click += [&](IMouse::Button button)
        {
            if (button == IMouse::Button::Left)
            {
                const bool want_capture_mouse = ImGui::GetCurrentContext()
                    ? ImGui::GetIO().WantCaptureMouse : false;
                if (!(_ui->is_cursor_over() || want_capture_mouse))
                {
                    if (_ui->show_context_menu())
                    {
                        _ui->set_show_context_menu(false);
                        return;
                    }

                    _ui->set_show_context_menu(false);

                    if (_compass_axis.has_value())
                    {
                        align_camera_to_axis(*_camera, _compass_axis.value());
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
                                set_camera_mode(ICamera::Mode::Orbit);
                                auto stored_pick = _current_pick;
                                stored_pick.position = target();
                                add_recent_orbit(stored_pick);
                            }
                        }
                    }
                }
            }
            else if (button == IMouse::Button::Right)
            {
                _ui->set_show_context_menu(false);
            }
        };

        _token_store += _mouse->mouse_click += [&](auto button)
        {
            if (button == input::IMouse::Button::Right && _current_pick.hit && _current_pick.type != PickResult::Type::Compass && _camera->idle_rotation())
            {
                _context_pick = _current_pick;
                _ui->set_show_context_menu(true);
                _camera_input.reset(true);
                _ui->set_remove_waypoint_enabled(_current_pick.type == PickResult::Type::Waypoint);
                _ui->set_hide_enabled(equals_any(_current_pick.type, PickResult::Type::Entity, PickResult::Type::Trigger, PickResult::Type::Light, PickResult::Type::Room, PickResult::Type::CameraSink, PickResult::Type::StaticMesh, PickResult::Type::SoundSource));
                _ui->set_mid_waypoint_enabled(_current_pick.type == PickResult::Type::Room && _current_pick.triangle.normal().y < 0);
                _ui->set_tile_filter_enabled(equals_any(_current_pick.type, PickResult::Type::Room, PickResult::Type::Entity, PickResult::Type::Trigger, PickResult::Type::Light, PickResult::Type::CameraSink));

                const auto level = _level.lock();
                if (_current_pick.type == PickResult::Type::Entity && level)
                {
                    const auto item = _current_pick.item.lock();
                    _ui->set_triggered_by(item ? item->triggers() : std::vector<std::weak_ptr<ITrigger>>{});
                }
                else if (_current_pick.type == PickResult::Type::CameraSink && level)
                {
                    const auto camera_sink = _current_pick.camera_sink.lock();
                    _ui->set_triggered_by(camera_sink ? camera_sink->triggers() : std::vector<std::weak_ptr<ITrigger>>{});
                }
                else 
                {
                    _ui->set_triggered_by({});
                }
            }
        };
    }

    void Viewer::update_camera()
    {
        _camera->update(_timer.elapsed(), _camera_input.movement());

        // TODO: Replace/Move
        if (auto level = _level.lock())
        {
            level->on_camera_moved();
        }
    }

    void Viewer::open(const std::weak_ptr<ILevel>& level, ILevel::OpenMode open_mode)
    {
        auto old_level = _level.lock();
        auto new_level = level.lock();

        if (old_level == new_level)
        {
            return;
        }

        _level = level;

        _level_token_store.clear();
        _level_token_store += new_level->on_alternate_mode_selected += [&](bool enabled) { set_alternate_mode(enabled); };
        _level_token_store += new_level->on_alternate_group_selected += [&](uint16_t group, bool enabled) { set_alternate_group(group, enabled); };

        new_level->set_show_triggers(_ui->toggle(Options::triggers));
        new_level->set_show_geometry(_ui->toggle(Options::geometry));
        new_level->set_show_water(_ui->toggle(Options::water));
        new_level->set_show_wireframe(_ui->toggle(Options::wireframe));
        new_level->set_show_bounding_boxes(_ui->toggle(Options::show_bounding_boxes));
        new_level->set_show_lights(_ui->toggle(Options::lights));
        new_level->set_show_items(_ui->toggle(Options::items));
        new_level->set_show_rooms(_ui->toggle(Options::rooms));
        new_level->set_show_camera_sinks(_ui->toggle(Options::camera_sinks));
        new_level->set_show_lighting(_ui->toggle(Options::lighting));
        new_level->set_show_sound_sources(_ui->toggle(Options::sound_sources));
        new_level->set_ng_plus(_ui->toggle(Options::ng_plus));
        new_level->set_show_animation(_ui->toggle(Options::animation));

        // Set up the views.
        auto rooms = new_level->rooms();

        _ui->set_level(new_level);
        window().set_title("trview - " + new_level->name());

        if (open_mode == ILevel::OpenMode::Full || !old_level)
        {
            _camera->reset();
            _ui->set_toggle(Options::highlight, false);
            _ui->set_toggle(Options::flip, false);
            _ui->set_toggle(Options::depth_enabled, false);
            _ui->set_scalar(Options::depth, 1);
            _measure->reset();
            _recent_orbits.clear();
            _recent_orbit_index = 0u;

            std::weak_ptr<IItem> lara;
            if (_settings.go_to_lara && find_last_item_by_type_id(*new_level, 0u, lara))
            {
                messages::send_select_item(_messaging, lara);
            }
            else
            {
                messages::send_select_room(_messaging, new_level->room(0));
            }

            // TODO: Does this need to happen? Might be done by the above.
            if (auto selected_room = new_level->selected_room().lock())
            {
                messages::send_select_room(_messaging, selected_room);
            }
        }
        else if (open_mode == ILevel::OpenMode::Reload && old_level)
        {
            new_level->set_alternate_mode(old_level->alternate_mode());
            new_level->set_neighbour_depth(old_level->neighbour_depth());
            new_level->set_highlight_mode(ILevel::RoomHighlightMode::Highlight, old_level->highlight_mode_enabled(ILevel::RoomHighlightMode::Highlight));
            new_level->set_highlight_mode(ILevel::RoomHighlightMode::Neighbours, old_level->highlight_mode_enabled(ILevel::RoomHighlightMode::Neighbours));

            for (const auto& group : new_level->alternate_groups())
            {
                new_level->set_alternate_group(group, old_level->alternate_group(group));
            }
        }

        // Reset UI buttons
        _ui->set_max_rooms(static_cast<uint32_t>(rooms.size()));
        _ui->set_use_alternate_groups(new_level->version() >= trlevel::LevelVersion::Tomb4);
        _ui->set_alternate_groups(new_level->alternate_groups());
        _ui->set_flip_enabled(new_level->any_alternates());
    }

    void Viewer::render()
    {
        _timer.update();
        update_camera();

        const auto mouse_pos = client_cursor_position(window());
        if (mouse_pos != _previous_mouse_pos || (_camera_moved || _camera_input.movement().LengthSquared() > 0))
        {
            _picking->pick(*_camera);
        }
        _previous_mouse_pos = mouse_pos;
        _camera_moved = false;

        _device->begin();
        _main_window->begin();
        _main_window->clear(Colour(_settings.background_colour));
        render_scene();

        _ui->set_camera_position(_camera->position());
        _ui->set_camera_rotation(_camera->rotation_yaw(), _camera->rotation_pitch());
    }

    void Viewer::render_ui()
    {
        _ui->render();
    }

    void Viewer::present(bool vsync)
    {
        _main_window->present(vsync);
    }

    bool Viewer::should_pick() const
    {
        const auto window = this->window();
        const auto level = _level.lock();
        return level && window_under_cursor() == window && !window_is_minimised(window) && !_ui->is_cursor_over() && !cursor_outside_window(window);
    }

    void Viewer::render_scene()
    {
        if (auto level = _level.lock())
        {
            level->render(*_camera, _show_selection);

            _sector_highlight->render(*_camera);
            _measure->render(*_camera);

            if (_show_route)
            {
                _route->render(*_camera, _show_selection);
            }

            level->render_transparency(*_camera);
            _compass->render(*_camera);
        }
    }

    void Viewer::set_camera_mode(ICamera::Mode camera_mode)
    {
        _camera_moved = true;
        _camera->set_mode(camera_mode);
        _ui->set_camera_mode(camera_mode);
    }

    void Viewer::set_camera_projection_mode(ProjectionMode projection_mode)
    {
        _camera->set_projection_mode(projection_mode);
        _ui->set_camera_projection_mode(projection_mode);
    }

    void Viewer::toggle_highlight()
    {
        if (auto level = _level.lock())
        {
            bool new_value = !level->highlight_mode_enabled(ILevel::RoomHighlightMode::Highlight);
            level->set_highlight_mode(ILevel::RoomHighlightMode::Highlight, new_value);
            _ui->set_toggle(Options::highlight, new_value);
        }
    }

    void Viewer::select_room(const std::weak_ptr<IRoom>& room)
    {
        if (const auto room_ptr = get_entity_and_sync_level(room))
        {
            if (room_ptr == _latest_room.lock())
            {
                return;
            }
            _latest_room = room;

            _was_alternate_select = false;
            _ui->set_selected_room(room_ptr);
            set_target(room_ptr->centre());
            if (_settings.auto_orbit)
            {
                set_camera_mode(ICamera::Mode::Orbit);
            }
        }
    }

    void Viewer::select_item(const std::weak_ptr<IItem>& item)
    {
        if (const auto item_ptr = get_entity_and_sync_level(item))
        {
            select_room(item_ptr->room());
            set_target(item_ptr->position());
            if (_settings.auto_orbit)
            {
                set_camera_mode(ICamera::Mode::Orbit);
            }
        }
    }

    void Viewer::select_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        if (const auto trigger_ptr = get_entity_and_sync_level(trigger))
        {
            select_room(trigger_ptr->room());
            set_target(trigger_ptr->position());
            if (_settings.auto_orbit)
            {
                set_camera_mode(ICamera::Mode::Orbit);
            }
        }
    }

    void Viewer::select_waypoint(const std::weak_ptr<IWaypoint>& waypoint)
    {
        if (auto waypoint_ptr = waypoint.lock())
        {
            set_target(waypoint_ptr->position());
            if (_settings.auto_orbit)
            {
                set_camera_mode(ICamera::Mode::Orbit);
            }
        }
    }

    void Viewer::set_route(const std::shared_ptr<IRoute>& route)
    {
        _route = route;
        _ui->set_route(route);
    }

    void Viewer::set_show_compass(bool value)
    {
        _compass->set_visible(value);
    }

    void Viewer::set_show_minimap(bool value)
    {
        _ui->set_show_minimap(value);
    }

    void Viewer::set_show_route(bool value)
    {
        _show_route = value;
    }

    void Viewer::set_show_selection(bool value)
    {
        _show_selection = value;
    }

    void Viewer::set_show_tools(bool value)
    {
        _measure->set_visible(value);
    }

    void Viewer::set_show_tooltip(bool value)
    {
        _ui->set_show_tooltip(value);
    }

    void Viewer::set_show_ui(bool value)
    {
        _ui->set_visible(value);
    }

    bool Viewer::ui_input_active() const
    {
        return _ui->is_input_active();
    }

    void Viewer::set_alternate_mode(bool enabled)
    {
        if (auto level = _level.lock())
        {
            _was_alternate_select = true;
            level->set_alternate_mode(enabled);
            _ui->set_toggle(Options::flip, enabled);
        }
    }

    void Viewer::toggle_alternate_mode()
    {
        const auto level = _level.lock();
        if (level && level->any_alternates())
        {
            set_alternate_mode(!level->alternate_mode());
        }
    }

    void Viewer::set_alternate_group(uint32_t group, bool enabled)
    {
        if (auto level = _level.lock())
        {
            if (std::ranges::contains(level->alternate_groups(), group))
            {
                _was_alternate_select = true;
                level->set_alternate_group(group, enabled);
                _ui->set_alternate_group(group, enabled);
            }
        }
    }

    bool Viewer::alternate_group(uint32_t group) const
    {
        if (const auto level = _level.lock())
        {
            return level->alternate_group(group);
        }
        return false;
    }

    void Viewer::resize_elements()
    {
        const auto size = window().size();
        if (size == Size())
        {
            return;
        }

        // Inform elements that need to know that the device has been resized.
        _camera->set_view_size(size);
        _ui->set_host_size_changed();
    }

    // Set up keyboard and mouse input for the camera.
    void Viewer::setup_camera_input()
    {
        using namespace input;

        _token_store += _mouse->mouse_down += [&](auto button) { _camera_input.mouse_down(button); };
        _token_store += _mouse->mouse_up += [&](auto button) { _camera_input.mouse_up(button); };
        _token_store += _mouse->mouse_move += [&](long x, long y) { _camera_input.mouse_move(x, y); };
        _token_store += _mouse->mouse_wheel += [&](int16_t scroll) 
        {
            if (window_under_cursor() == window())
            {
                if (ImGui::GetCurrentContext() != nullptr)
                {
                    auto& io = ImGui::GetIO();
                    if (!io.WantCaptureMouse)
                    {
                        _ui->set_show_context_menu(false);
                        _camera_input.mouse_scroll(scroll);
                    }
                }
            }
        };

        _token_store += _camera_input.on_rotate += [&](float x, float y)
        {
            if (_ui->is_cursor_over())
            {
                return;
            }

            _camera_moved = true;
            _ui->set_show_context_menu(false);

            const float low_sensitivity = 200.0f;
            const float high_sensitivity = 25.0f;
            const float sensitivity = low_sensitivity + (high_sensitivity - low_sensitivity) * _settings.camera_sensitivity;
            _camera->set_rotation_yaw(_camera->rotation_yaw() + x / sensitivity);
            _camera->set_rotation_pitch(_camera->rotation_pitch() - y / sensitivity);
            if (auto level = _level.lock())
            {
                level->on_camera_moved();
            }
        };

        _token_store += _camera_input.on_zoom += [&](float zoom)
        {
            if (_ui->is_cursor_over())
            {
                return;
            }
            
            _camera_moved = true;
            if (camera_mode() == ICamera::Mode::Orbit)
            {
                _camera->set_zoom(_camera->zoom() + zoom);
                if (auto level = _level.lock())
                {
                    level->on_camera_moved();
                }
            }
            else if (_camera->projection_mode() == ProjectionMode::Orthographic)
            {
                _camera->set_zoom(_camera->zoom() + zoom);
                if (auto level = _level.lock())
                {
                    level->on_camera_moved();
                }
            }
        };

        _token_store += _camera_input.on_pan += [&](bool vertical, float x, float y)
        {
            if (_ui->is_cursor_over() || camera_mode() != ICamera::Mode::Orbit)
            {
                return;
            }

            _camera_moved = true;
            _ui->set_show_context_menu(false);

            using namespace DirectX::SimpleMath;

            if (_camera->projection_mode() == ProjectionMode::Perspective)
            {
                if (vertical)
                {
                    set_target(target() + 0.05f * Vector3::Up * y * (_settings.invert_vertical_pan ? -1.0f : 1.0f));
                }
                else
                {
                    // Rotate forward and right by the camera yaw...
                    const auto rotation = Matrix::CreateRotationY(_camera->rotation_yaw());
                    const auto forward = Vector3::Transform(Vector3::Forward, rotation);
                    const auto right = Vector3::Transform(Vector3::Right, rotation);

                    // Add them on to the position.
                    const auto movement = 0.05f * (forward * -y + right * -x);
                    set_target(target() + movement);
                }
            }
            else
            {
                auto rotate = Matrix::CreateFromYawPitchRoll(_camera->rotation_yaw(), _camera->rotation_pitch(), 0);
                set_target(target() + 0.05f * Vector3::Transform(Vector3(-x, y * (_settings.invert_vertical_pan ? -1.0f : 1.0f), 0), rotate));
            }

            if (auto level = _level.lock())
            {
                level->on_camera_moved();
            }
        };

        _token_store += _camera_input.on_mode_change += [&](auto mode) { set_camera_mode(mode); };
    }

    void Viewer::set_show_triggers(bool show)
    {
        if (auto level = _level.lock())
        {
            level->set_show_triggers(show);
        }
        set_toggle(Options::triggers, show);
    }

    void Viewer::toggle_show_triggers()
    {
        if (auto level = _level.lock())
        {
            set_show_triggers(!level->show_triggers());
        }
    }

    void Viewer::set_show_items(bool show)
    {
        if (auto level = _level.lock())
        {
            level->set_show_items(show);
        }
        set_toggle(Options::items, show);
    }

    void Viewer::toggle_show_items()
    {
        if (auto level = _level.lock())
        {
            set_show_items(!level->show_items());
        }
    }

    void Viewer::set_show_geometry(bool show)
    {
        if (auto level = _level.lock())
        {
            level->set_show_geometry(show);
        }
        set_toggle(Options::geometry, show);
    }

    void Viewer::toggle_show_geometry()
    {
        if (auto level = _level.lock())
        {
            set_show_geometry(!level->show_geometry());
        }
    }

    void Viewer::toggle_show_lights()
    {
        if (auto level = _level.lock())
        {
            set_show_lights(!level->show_lights());
        }
    }

    void Viewer::set_show_water(bool show)
    {
        if (auto level = _level.lock())
        {
            level->set_show_water(show);
        }
        set_toggle(Options::water, show);
    }

    void Viewer::set_show_wireframe(bool show)
    {
        if (auto level = _level.lock())
        {
            level->set_show_wireframe(show);
        }
        set_toggle(Options::wireframe, show);
    }

    void Viewer::set_show_bounding_boxes(bool show)
    {
        if (auto level = _level.lock())
        {
            level->set_show_bounding_boxes(show);
        }
        set_toggle(Options::show_bounding_boxes, show);
    }

    void Viewer::set_show_lights(bool show)
    {
        if (auto level = _level.lock())
        {
            level->set_show_lights(show);
        }
        set_toggle(Options::lights, show);
    }

    std::weak_ptr<IRoom> Viewer::room_from_pick(const PickResult& pick) const
    {
        const auto level = _level.lock();
        if (!level)
        {
            return {};
        }

        switch (pick.type)
        {
            case PickResult::Type::Room:
            {
                return pick.room;
            }
            case PickResult::Type::Entity:
            {
                if (auto item = pick.item.lock())
                {
                    return item->room();
                }
                break;
            }
            case PickResult::Type::Trigger:
            {
                if (const auto trigger = pick.trigger.lock())
                {
                    return trigger->room();
                }
                break;
            }
            case PickResult::Type::Waypoint:
            {
                if (const auto waypoint = pick.waypoint.lock())
                {
                    return level->room(waypoint->room());
                }
                break;
            }
        }

        return level->selected_room();
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
        const auto level = _level.lock();
        switch (pick.type)
        {
        case PickResult::Type::Room:
        {
            messages::send_select_room(_messaging, pick.room);
            if (pick.override_centre)
            {
                set_target(pick.position);
            }
            break;
        }
        case PickResult::Type::Entity:
        {
            messages::send_select_item(_messaging, pick.item);
            break;
        }
        case PickResult::Type::Trigger:
        {
            messages::send_select_trigger(_messaging, pick.trigger);
            break;
        }
        case PickResult::Type::Waypoint:
        {
            messages::send_select_waypoint(_messaging, pick.waypoint);
            break;
        }
        case PickResult::Type::Light:
        {
            messages::send_select_light(_messaging, pick.light);
            break;
        }
        case PickResult::Type::CameraSink:
        {
            messages::send_select_camera_sink(_messaging, pick.camera_sink);
            break;
        }
        case PickResult::Type::StaticMesh:
        {
            messages::send_select_static_mesh(_messaging, pick.static_mesh);
            break;
        }
        case PickResult::Type::Scriptable:
        {
            if (auto scriptable = pick.scriptable.lock())
            {
                scriptable->click();
            }
            break;
        }
        case PickResult::Type::SoundSource:
        {
            messages::send_select_sound_source(_messaging, pick.sound_source);
            break;
        }
        case PickResult::Type::FlybyNode:
        {
            messages::send_select_flyby_node(_messaging, pick.flyby_node);
            break;
        }
        }
    }

    void Viewer::apply_camera_settings()
    {
        _camera->set_settings(_settings);
    }

    std::weak_ptr<ICamera> Viewer::camera() const
    {
        return _camera;
    }

    ICamera::Mode Viewer::camera_mode() const
    {
        return _camera->mode();
    }

    void Viewer::select_light(const std::weak_ptr<ILight>& light)
    {
        if (const auto light_ptr = get_entity_and_sync_level(light))
        {
            select_room(light_ptr->room());
            set_target(light_ptr->position());
            if (_settings.auto_orbit)
            {
                set_camera_mode(ICamera::Mode::Orbit);
            }
        }
    }

    std::optional<int> Viewer::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        switch (message)
        {
            case WM_ACTIVATE:
            {
                _camera_input.reset_input();
                break;
            }
            case WM_COMMAND:
            {
                switch (LOWORD(wParam))
                {
                    case ID_WINDOWS_CAMERA_POSITION:
                    {
                        _settings.camera_position_window = true;
                        messages::send_settings(_messaging, _settings);
                        _ui->set_show_camera_position(true);
                        break;
                    }
                    case ID_WINDOWS_RESET_LAYOUT:
                    {
                        _settings.camera_position_window = true;
                        messages::send_settings(_messaging, _settings);
                        _ui->reset_layout();
                        _ui->set_show_camera_position(true);
                        break;
                    }
                }
            }
            
        }
        return {};
    }

    DirectX::SimpleMath::Vector3 Viewer::target() const
    {
        return _camera->target();
    }

    void Viewer::set_target(const DirectX::SimpleMath::Vector3& target)
    {
        _camera->set_target(target);
    }

    void Viewer::set_show_rooms(bool show)
    {
        if (auto level = _level.lock())
        {
            level->set_show_rooms(show);
        }
        set_toggle(Options::rooms, show);
    }

    void Viewer::select_sector(const std::weak_ptr<ISector>& sector)
    {
        if (const auto sector_ptr = sector.lock())
        {
            if (get_entity_and_sync_level(sector_ptr->room()))
            {
                set_sector_highlight(sector_ptr);
                _ui->set_minimap_highlight(sector_ptr->x(), sector_ptr->z());
            }
        }
    }

    void Viewer::set_sector_highlight(const std::shared_ptr<ISector>& sector)
    {
        if (!sector)
        {
            _sector_highlight->set_sector(nullptr, Matrix::Identity);
            return;
        }

        const auto room = sector->room().lock();
        if (!room)
        {
            return;
        }
        
        const auto room_info = room->info();
        _sector_highlight->set_sector(sector,
            Matrix::CreateTranslation(room_info.x / trlevel::Scale_X, 0, room_info.z / trlevel::Scale_Z));
    }

    void Viewer::select_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink)
    {
        if (const auto camera_sink_ptr = get_entity_and_sync_level(camera_sink))
        {
            select_room(actual_room(*camera_sink_ptr));
            set_target(camera_sink_ptr->position());
            if (_settings.auto_orbit)
            {
                set_camera_mode(ICamera::Mode::Orbit);
            }
        }
    }

    void Viewer::set_show_camera_sinks(bool show)
    {
        if (auto level = _level.lock())
        {
            level->set_show_camera_sinks(show);
        }
        set_toggle(Options::camera_sinks, show);
    }

    void Viewer::toggle_show_camera_sinks()
    {
        if (auto level = _level.lock())
        {
            set_show_camera_sinks(!level->show_camera_sinks());
        }
    }

    void Viewer::set_show_lighting(bool show)
    {
        if (auto level = _level.lock())
        {
            level->set_show_lighting(show);
        }
        set_toggle(Options::lighting, show);
    }

    void Viewer::toggle_show_lighting()
    {
        if (auto level = _level.lock())
        {
            set_show_lighting(!level->show_lighting());
        }
    }

    void Viewer::select_static_mesh(const std::weak_ptr<IStaticMesh>& static_mesh)
    {
        if (const auto static_mesh_ptr = get_entity_and_sync_level(static_mesh))
        {
            select_room(static_mesh_ptr->room());
            set_target(static_mesh_ptr->position());
            if (_settings.auto_orbit)
            {
                set_camera_mode(ICamera::Mode::Orbit);
            }
        }
    }

    void Viewer::set_toggle(const std::string& name, bool value)
    {
        _ui->set_toggle(name, value);
        _settings.toggles[name] = value;
        messages::send_settings(_messaging, _settings);
    }

    void Viewer::set_show_sound_sources(bool show)
    {
        if (auto level = _level.lock())
        {
            level->set_show_sound_sources(show);
        }
        set_toggle(Options::sound_sources, show);
    }

    void Viewer::select_sound_source(const std::weak_ptr<ISoundSource>& sound_source)
    {
        if (const auto sound_source_ptr = get_entity_and_sync_level(sound_source))
        {
            set_target(sound_source_ptr->position());
            if (_settings.auto_orbit)
            {
                set_camera_mode(ICamera::Mode::Orbit);
            }
        }
    }

    void Viewer::set_ng_plus(bool show)
    {
        if (auto level = _level.lock())
        {
            level->set_ng_plus(show);
        }
        set_toggle(Options::ng_plus, show);
    }

    std::weak_ptr<ILevel> Viewer::level() const
    {
        return _level;
    }

    void Viewer::select_flyby_node(const std::weak_ptr<IFlybyNode>& flyby_node)
    {
        if (const auto flyby_node_ptr = get_entity_and_sync_level(flyby_node))
        {
            if (auto level = _level.lock())
            {
                select_room(level->room(flyby_node_ptr->room()));
            }
            set_target(flyby_node_ptr->position());
            if (_settings.auto_orbit)
            {
                set_camera_mode(ICamera::Mode::Orbit);
            }
        }
    }

    void Viewer::set_show_animation(bool show)
    {
        if (auto level = _level.lock())
        {
            level->set_show_animation(show);
        }
        set_toggle(Options::animation, show);
    }

    void Viewer::receive_message(const Message& message)
    {
        if (auto selected_room = messages::read_select_room(message))
        {
            select_room(selected_room.value());
        }
        else if (auto selected_item = messages::read_select_item(message))
        {
            select_item(selected_item.value());
        }
        else if (auto selected_light = messages::read_select_light(message))
        {
            select_light(selected_light.value());
        }
        else if (auto settings = messages::read_settings(message))
        {
            _settings = settings.value();
            apply_camera_settings();
        }
        else if (auto selected_trigger = messages::read_select_trigger(message))
        {
            select_trigger(selected_trigger.value());
        }
        else if (auto selected_camera_sink = messages::read_select_camera_sink(message))
        {
            select_camera_sink(selected_camera_sink.value());
        }
        else if (auto selected_sound_source = messages::read_select_sound_source(message))
        {
            select_sound_source(selected_sound_source.value());
        }
        else if (auto selected_static_mesh = messages::read_select_static_mesh(message))
        {
            select_static_mesh(selected_static_mesh.value());
        }
        else if (auto selected_flyby_node = messages::read_select_flyby_node(message))
        {
            select_flyby_node(selected_flyby_node.value());
        }
        else if (auto hovered_sector = messages::read_hover_sector(message))
        {
            select_sector(hovered_sector.value());
        }
        else if (auto selected_sector = messages::read_select_sector(message))
        {
            if (const auto s = selected_sector.value().lock())
            {
                if (const auto r = s->room().lock())
                {
                    select_room(r);
                    set_target(r->sector_centroid(s));
                }
            }
        }
        else if (auto selected_waypoint = messages::read_select_waypoint(message))
        {
            select_waypoint(selected_waypoint.value());
        }
        else if (auto route = messages::read_route(message))
        {
            set_route(route.value().lock());
        }
    }

    void Viewer::initialise()
    {
        messages::get_selected_room(_messaging, weak_from_this());
        messages::get_selected_item(_messaging, weak_from_this());
        messages::get_selected_light(_messaging, weak_from_this());
        messages::get_selected_trigger(_messaging, weak_from_this());
        messages::get_selected_camera_sink(_messaging, weak_from_this());
        messages::get_selected_sound_source(_messaging, weak_from_this());
        messages::get_selected_static_mesh(_messaging, weak_from_this());
        messages::get_selected_flyby_node(_messaging, weak_from_this());
    }
}
