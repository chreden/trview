#include "Viewer.h"

#include <trlevel/trlevel.h>
#include <trview.graphics/ShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include <trview.graphics/DeviceWindow.h>

#include <trview.graphics/RenderTargetStore.h>
#include <trview.graphics/Sprite.h>
#include <trview.graphics/ViewportStore.h>
#include <trview.input/WindowTester.h>

#include "DefaultTextures.h"
#include "DefaultShaders.h"
#include "DefaultFonts.h"
#include <trview.app/Graphics/TextureStorage.h>
#include <trview.app/Elements/TypeNameLookup.h>
#include "ResourceHelper.h"
#include "resource.h"

#include <trview.common/Strings.h>
#include <fstream>
#include <iostream>

#include <trview.lau/drm.h>

namespace trview
{
    namespace
    {
        const float _CAMERA_MOVEMENT_SPEED_MULTIPLIER = 23.0f;
        const float _CAMERA_MOVEMENT_SPEED_DEFAULT = 0.5f;
    }

    Viewer::Viewer(const Window& window)
        : _window(window), _shortcuts(window), _camera(window.size()), _free_camera(window.size()),
        _timer(default_time_source()), _keyboard(window), _mouse(window, std::make_unique<input::WindowTester>(window)), _level_switcher(window),
        _window_resizer(window), _recent_files(window), _file_dropper(window), _alternate_group_toggler(window),
        _view_menu(window), _update_checker(window), _menu_detector(window)
    {
        _update_checker.check_for_updates();

        _settings = load_user_settings();

        Resource type_list = get_resource_memory(IDR_TYPE_NAMES, L"TEXT");
        _type_name_lookup = std::make_unique<TypeNameLookup>(std::string(type_list.data, type_list.data + type_list.size));

        _shader_storage = std::make_unique<graphics::ShaderStorage>();
        load_default_shaders(_device, *_shader_storage.get());

        _scene_target = std::make_unique<graphics::RenderTarget>(_device, static_cast<uint32_t>(window.size().width), static_cast<uint32_t>(window.size().height), graphics::RenderTarget::DepthStencilMode::Enabled);
        _scene_sprite = std::make_unique<graphics::Sprite>(_device, *_shader_storage, window.size());
        _token_store += _free_camera.on_view_changed += [&]() { _scene_changed = true; };
        _token_store += _camera.on_view_changed += [&]() { _scene_changed = true; };

        load_default_fonts(_device, _font_factory);

        _main_window = _device.create_for_window(window);
        _items_windows = std::make_unique<ItemsWindowManager>(_device, *_shader_storage.get(), _font_factory, window, _shortcuts);
        if (_settings.items_startup)
        {
            _items_windows->create_window();
        }
        _token_store += _items_windows->on_item_selected += [this](const auto& item) { select_item(item); };
        _token_store += _items_windows->on_trigger_selected += [this](const auto& trigger) { select_trigger(trigger); };
        _token_store += _items_windows->on_add_to_route += [this](const auto& item)
        {
            uint32_t new_index = _route->insert(item.position(), item.room(), Waypoint::Type::Entity, item.number());
            _route_window_manager->set_route(_route.get());
            select_waypoint(new_index);
        };

        _triggers_windows = std::make_unique<TriggersWindowManager>(_device, *_shader_storage.get(), _font_factory, window, _shortcuts);
        if (_settings.triggers_startup)
        {
            _triggers_windows->create_window();
        }
        _token_store += _triggers_windows->on_item_selected += [this](const auto& item) { select_item(item); };
        _token_store += _triggers_windows->on_trigger_selected += [this](const auto& trigger) { select_trigger(trigger); };
        _token_store += _triggers_windows->on_add_to_route += [this](const auto& trigger)
        {
            uint32_t new_index = _route->insert(trigger->position(), trigger->room(), Waypoint::Type::Trigger, trigger->number());
            _route_window_manager->set_route(_route.get());
            select_waypoint(new_index);
        };

        _rooms_windows = std::make_unique<RoomsWindowManager>(_device, *_shader_storage.get(), _font_factory, window, _shortcuts);
        if (_settings.rooms_startup)
        {
            _rooms_windows->create_window();
        }

        _token_store += _rooms_windows->on_room_selected += [this](const auto& room) { select_room(room); };
        _token_store += _rooms_windows->on_item_selected += [this](const auto& item) { select_item(item); };
        _token_store += _rooms_windows->on_trigger_selected += [this](const auto& trigger) { select_trigger(trigger); };

        _token_store += _level_switcher.on_switch_level += [=](const auto& file) { open(file); };
        _token_store += on_file_loaded += [&](const auto& file) { _level_switcher.open_file(file); };

        _token_store += _window_resizer.on_resize += [=]()
        {
            _main_window->resize();
            resize_elements();
        };

        _token_store += _recent_files.on_file_open += [=](const auto& file) { open(file); };
        _recent_files.set_recent_files(_settings.recent_files);
        _token_store += on_recent_files_changed += [&](const auto& files) { _recent_files.set_recent_files(files); };

        _token_store += _file_dropper.on_file_dropped += [&](const auto& file) { open(file); };

        _token_store += _alternate_group_toggler.on_alternate_group += [&](uint32_t group)
        {
            if (!_ui->is_input_active())
            {
                set_alternate_group(group, !alternate_group(group));
            }
        };

        initialise_input();

        _texture_storage = std::make_unique<TextureStorage>(_device);
        load_default_textures(_device, *_texture_storage.get());

        _ui = std::make_unique<ViewerUI>(_window, _device, *_shader_storage, _font_factory, *_texture_storage, _shortcuts);
        _token_store += _ui->on_ui_changed += [&]() {_ui_changed = true; };
        _token_store += _ui->on_select_item += [&](uint32_t index)
        {
            if (_level && index < _level->items().size())
            {
                select_item(_level->items()[index]);
            }
        };
        _token_store += _ui->on_select_room += [&](uint32_t room) { select_room(room); };
        _token_store += _ui->on_highlight += [&](bool) { toggle_highlight(); };
        _token_store += _ui->on_show_hidden_geometry += [&](bool value) { set_show_hidden_geometry(value); };
        _token_store += _ui->on_show_water += [&](bool value) { set_show_water(value); };
        _token_store += _ui->on_show_triggers += [&](bool value) { set_show_triggers(value); };
        _token_store += _ui->on_flip += [&](bool value) { set_alternate_mode(value); };
        _token_store += _ui->on_alternate_group += [&](uint32_t group, bool value) { set_alternate_group(group, value); };
        _token_store += _ui->on_depth += [&](bool value) { if (_level) { _level->set_highlight_mode(Level::RoomHighlightMode::Neighbours, value); } };
        _token_store += _ui->on_depth_level_changed += [&](int32_t value) { if (_level) { _level->set_neighbour_depth(value); } };
        _token_store += _ui->on_camera_reset += [&]() { _camera.reset(); };
        _token_store += _ui->on_camera_mode += [&](CameraMode mode) { set_camera_mode(mode); };
        _token_store += _ui->on_camera_projection_mode += [&](ProjectionMode mode) { set_camera_projection_mode(mode); };
        _token_store += _ui->on_camera_sensitivity += [&](float value) { _settings.camera_sensitivity = value; };
        _token_store += _ui->on_camera_movement_speed += [&](float value) { _settings.camera_movement_speed = value; };
        _token_store += _ui->on_sector_hover += [&](const std::shared_ptr<Sector>& sector)
        {
            if (_level)
            {
                const auto room_info = _level->room_info(_level->selected_room());
                _sector_highlight.set_sector(sector,
                    DirectX::SimpleMath::Matrix::CreateTranslation(room_info.x / trlevel::Scale_X, 0, room_info.z / trlevel::Scale_Z));
                _scene_changed = true;
            }
        };
        _token_store += _ui->on_add_waypoint += [&]()
        {
            auto type = _context_pick.type == PickResult::Type::Entity ? Waypoint::Type::Entity : _context_pick.type == PickResult::Type::Trigger ? Waypoint::Type::Trigger : Waypoint::Type::Position;
            uint32_t new_index = _route->insert(_context_pick.position, room_from_pick(_context_pick), type, _context_pick.index);
            _route_window_manager->set_route(_route.get());
            select_waypoint(new_index);
        };
        _token_store += _ui->on_remove_waypoint += [&]() { remove_waypoint(_context_pick.index); };
        _token_store += _ui->on_orbit += [&]()
        {
            select_room(room_from_pick(_context_pick), true);
            _target = _context_pick.position;

            auto stored_pick = _context_pick;
            stored_pick.override_centre = true;
            stored_pick.type = PickResult::Type::Room;
            add_recent_orbit(stored_pick);
        };
        _token_store += _ui->on_settings += [&](auto settings) { _settings = settings; };
        _token_store += _ui->on_tool_selected += [&](auto tool) { _active_tool = tool; _measure->reset(); };
        _token_store += _ui->on_camera_position += [&](const auto& position)
        {
            if (_camera_mode == CameraMode::Orbit)
            {
                set_camera_mode(CameraMode::Free);
            }
            _free_camera.set_position(position);
        };

        _token_store += _ui->on_command += [&](const auto& command)
        {
            _lua.execute(to_utf8(command));
        };

        _token_store += _lua.on_text += [&](const auto& text)
        {
            _ui->print_console(text);
        };

        _ui->set_settings(_settings);

        _ui->set_camera_mode(CameraMode::Orbit);
        _ui->set_camera_sensitivity(_settings.camera_sensitivity);
        _ui->set_camera_movement_speed(_settings.camera_movement_speed == 0 ? _CAMERA_MOVEMENT_SPEED_DEFAULT : _settings.camera_movement_speed);

        _measure = std::make_unique<Measure>(_device);
        _compass = std::make_unique<Compass>(_device, *_shader_storage);
        _route = std::make_unique<Route>(_device, *_shader_storage);

        _token_store += _measure->on_visible += [&](bool show) { _ui->set_show_measure(show); };
        _token_store += _measure->on_position += [&](auto pos) { _ui->set_measure_position(pos); };
        _token_store += _measure->on_distance += [&](float distance) { _ui->set_measure_distance(distance); };

        _route_window_manager = std::make_unique<RouteWindowManager>(_device, *_shader_storage, _font_factory, window, _shortcuts);
        _token_store += _route_window_manager->on_waypoint_selected += [&](auto index)
        {
            select_waypoint(index);
        };
        _token_store += _route_window_manager->on_item_selected += [&](const auto& item) { select_item(item); };
        _token_store += _route_window_manager->on_trigger_selected += [&](const auto& trigger) { select_trigger(trigger); };
        _token_store += _route_window_manager->on_route_import += [&](const std::string& path)
        {
            auto route = import_route(_device, *_shader_storage, path);
            if (route)
            {
                _route = std::move(route);
                _route_window_manager->set_route(_route.get());
                _scene_changed = true;
            }
        };
        _token_store += _route_window_manager->on_route_export += [&](const std::string& path)
        {
            export_route(*_route, path);
        };
        _token_store += _route_window_manager->on_waypoint_deleted += [&](auto index)
        {
            remove_waypoint(index);
        };
        _token_store += _route_window_manager->on_colour_changed += [&](const Colour& colour)
        {
            _route->set_colour(colour);
            _scene_changed = true;
        };

        _token_store += _view_menu.on_show_minimap += [&](bool show) { _ui->set_show_minimap(show); _ui_changed = true; };
        _token_store += _view_menu.on_show_tooltip += [&](bool show) { _ui->set_show_tooltip(show); _ui_changed = true; };
        _token_store += _view_menu.on_show_ui += [&](bool show) { _ui->set_visible(show); _ui_changed = true; };
        _token_store += _view_menu.on_show_compass += [&](bool show) { _compass->set_visible(show); _scene_changed = true; };
        _token_store += _view_menu.on_show_selection += [&](bool show) { _show_selection = show; _scene_changed = true; };
        _token_store += _view_menu.on_show_route += [&](bool show) { _show_route = show; _scene_changed = true; };
        _token_store += _view_menu.on_show_tools += [&](bool show) { _measure->set_visible(show); _scene_changed = true; };
        _token_store += _view_menu.on_colour_change += [&](Colour colour) 
        { 
            _settings.background_colour = static_cast<uint32_t>(colour);
            _scene_changed = true; 
        };

        _picking = std::make_unique<Picking>();
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
                        info = _level->room(_current_pick.index)->info();
                    }
                    else if (_current_pick.type == PickResult::Type::Trigger)
                    {
                        auto trigger = _level->triggers()[_current_pick.index];
                        if (trigger->room() == _level->selected_room())
                        {
                            info = _level->room(trigger->room())->info();
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

    Viewer::~Viewer()
    {
        save_user_settings(_settings);
    }

    UserSettings Viewer::settings() const
    {
        return _settings;
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
            _token_store += _shortcuts.add_shortcut(control, key) += [&, fn]()
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
        add_shortcut(false, VK_LEFT, [&]() { select_previous_waypoint(); });
        add_shortcut(false, VK_RIGHT, [&]() { select_next_waypoint(); });
        add_shortcut(false, VK_DELETE, [&]() { remove_waypoint(_route->selected_waypoint()); });
        add_shortcut(false, VK_F1, [&]() { _ui->toggle_settings_visibility(); });
        add_shortcut(false, 'H', [&]() { toggle_highlight(); });
        add_shortcut(false, VK_INSERT, [&]()
        {
            // Reset the camera to defaults.
            _camera.set_rotation_yaw(0.f);
            _camera.set_rotation_pitch(-0.78539f);
            _camera.set_zoom(8.f);
        });

        _token_store += _keyboard.on_key_down += [&](uint16_t key, bool control, bool shift)
        {
            if (!_ui->is_input_active())
            {
                _camera_input.key_down(key, control);
            }
        };

        setup_camera_input();

        using namespace input;

        _token_store += _mouse.mouse_click += [&](Mouse::Button button)
        {
            if (button == Mouse::Button::Left)
            {
                if (!_ui->is_cursor_over())
                {
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
                else if (std::shared_ptr<Sector> sector = _ui->current_minimap_sector())
                {
                    // Select the trigger (if it is a trigger).
                    const auto triggers = _level->triggers();
                    auto trigger = std::find_if(triggers.begin(), triggers.end(),
                        [&](auto t)
                    {
                        return t->room() == sector->room() && t->sector_id() == sector->id();
                    });

                    if (trigger == triggers.end() || (GetAsyncKeyState(VK_CONTROL) & 0x8000))
                    {
                        if (sector->flags & SectorFlag::Portal)
                        {
                            select_room(sector->portal());
                        }
                        else if (!_settings.invert_map_controls && (sector->flags & SectorFlag::RoomBelow))
                        {
                            select_room(sector->room_below());
                        }
                        else if (_settings.invert_map_controls && (sector->flags & SectorFlag::RoomAbove))
                        {
                            select_room(sector->room_above());
                        }
                    }
                    else
                    {
                        select_trigger(*trigger);
                    }
                }
            }
            else if (button == Mouse::Button::Right)
            {
                _ui->set_show_context_menu(false);

                if (auto sector = _ui->current_minimap_sector())
                {
                    if (!_settings.invert_map_controls && (sector->flags & SectorFlag::RoomAbove))
                    {
                        select_room(sector->room_above());
                    }
                    else if (_settings.invert_map_controls && (sector->flags & SectorFlag::RoomBelow))
                    {
                        select_room(sector->room_below());
                    }
                }
            }
        };

        _token_store += _mouse.mouse_click += [&](auto button)
        {
            if (button == input::Mouse::Button::Right && _current_pick.hit && _current_pick.type != PickResult::Type::Compass)
            {
                _context_pick = _current_pick;
                _ui->set_show_context_menu(true);
                _ui->set_remove_waypoint_enabled(_current_pick.type == PickResult::Type::Waypoint);
            }
        };
    }

    void Viewer::update_camera()
    {
        if (_camera_mode == CameraMode::Free || _camera_mode == CameraMode::Axis)
        {
            const float Speed = std::max(0.01f, _settings.camera_movement_speed) * _CAMERA_MOVEMENT_SPEED_MULTIPLIER;
            _free_camera.move(_camera_input.movement() * _timer.elapsed() * Speed);

            if (_level)
            {
                _level->on_camera_moved();
            }
        }

        current_camera().update(_timer.elapsed());
    }

    template <typename T>
    T read(std::istream& file)
    {
        T value;
        read<T>(file, value);
        return value;
    }

    template < typename T >
    void read(std::istream& file, T& value)
    {
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
    }

    void Viewer::open(const std::string& filename)
    {
        std::unique_ptr<trlevel::ILevel> new_level;
        try
        {
            new_level = trlevel::load_level(filename);
        }
        catch(...)
        {
            MessageBox(_window.window(), L"Failed to load level", L"Error", MB_OK);
            return;
        }

        on_file_loaded(filename);
        _settings.add_recent_file(filename);
        on_recent_files_changed(_settings.recent_files);
        save_user_settings(_settings);

        _level = std::make_unique<Level>(_device, *_shader_storage.get(), std::move(new_level), *_type_name_lookup);
        _token_store += _level->on_room_selected += [&](uint16_t room) { select_room(room); };
        _token_store += _level->on_alternate_mode_selected += [&](bool enabled) { set_alternate_mode(enabled); };
        _token_store += _level->on_alternate_group_selected += [&](uint16_t group, bool enabled) { set_alternate_group(group, enabled); };
        _token_store += _level->on_level_changed += [&]() { _scene_changed = true; };

        _items_windows->set_items(_level->items());
        _items_windows->set_triggers(_level->triggers());
        _triggers_windows->set_items(_level->items());
        _triggers_windows->set_triggers(_level->triggers());
        _route_window_manager->set_items(_level->items());
        _route_window_manager->set_triggers(_level->triggers());
        _rooms_windows->set_items(_level->items());
        _rooms_windows->set_triggers(_level->triggers());
        _rooms_windows->set_rooms(_level->rooms());

        _level->set_show_triggers(_ui->show_triggers());
        _level->set_show_hidden_geometry(_ui->show_hidden_geometry());
        _level->set_show_water(_ui->show_water());

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
            select_item(lara);
        }
        else
        {
            select_room(0);
        }

        _ui->set_depth_enabled(false);
        _ui->set_depth_level(1);

        // Strip the last part of the path away.
        auto last_index = std::min(filename.find_last_of('\\'), filename.find_last_of('/'));
        auto name = last_index == filename.npos ? filename : filename.substr(std::min(last_index + 1, filename.size()));
        _ui->set_level(name, _level->version());
        _window.set_title("trview - " + name);
        _measure->reset();
        _route->clear();

        _route_window_manager->set_route(_route.get());

        _recent_orbits.clear();
        _recent_orbit_index = 0u;

        _scene_changed = true;
    }

    void Viewer::render()
    {
        // If minimised, don't render like crazy. Sleep so we don't hammer the CPU either.
        if (window_is_minimised(_window))
        {
            Sleep(1);
            return;
        }

        _timer.update();
        update_camera();

        if (_mouse_changed || _scene_changed)
        {
            _picking->pick(_window, current_camera());
            _mouse_changed = false;
        }

        if (_scene_changed || _ui_changed)
        {
            _device.begin();
            _main_window->begin();
            _main_window->clear(Colour(_settings.background_colour));

            if (_scene_changed)
            {
                _scene_target->clear(_device.context(), Colour::Transparent);

                graphics::RenderTargetStore rs_store(_device.context());
                graphics::ViewportStore vp_store(_device.context());
                _scene_target->apply(_device.context());

                render_scene();
                _scene_changed = false;
            }

            _scene_sprite->render(_device.context(), _scene_target->texture(), 0, 0, _window.size().width, _window.size().height);
            _ui->set_camera_position(current_camera().position());

            _ui->render(_device);
            _ui_changed = false;

            _main_window->present(_settings.vsync);
        }

        _items_windows->render(_device, _settings.vsync);
        _triggers_windows->render(_device, _settings.vsync);
        _route_window_manager->render(_device, _settings.vsync);
        _rooms_windows->render(_device, _settings.vsync);
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
            _level->render(_device, current_camera(), _show_selection);
            _sector_highlight.render(_device, current_camera(), _level->texture_storage());

            _measure->render(_device.context(), current_camera(), _level->texture_storage());

            if (_show_route)
            {
                _route->render(_device, current_camera(), _level->texture_storage());
            }

            _level->render_transparency(_device, current_camera());
            _compass->render(_device, current_camera(), _level->texture_storage());
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

    void Viewer::select_room(uint32_t room, bool force_orbit)
    {
        if (!_level || room >= _level->number_of_rooms())
        {
            return;
        }

        _level->set_selected_room(static_cast<uint16_t>(room));
        _ui->set_selected_room(_level->room(_level->selected_room()));

        if (force_orbit || (_settings.auto_orbit && !_was_alternate_select))
        {
            set_camera_mode(CameraMode::Orbit);
        }

        _was_alternate_select = false;
        _target = _level->room(_level->selected_room())->centre();

        _items_windows->set_room(room);
        _triggers_windows->set_room(room);
        _rooms_windows->set_room(room);
    }

    void Viewer::select_item(const Item& item)
    {
        if (!_level || item.number() >= _level->items().size())
        {
            return;
        }

        select_room(item.room());
        _target = item.position();
        _level->set_selected_item(item.number());
        _items_windows->set_selected_item(item);
        _rooms_windows->set_selected_item(item);
    }

    void Viewer::select_trigger(const Trigger* const trigger)
    {
        if (_level)
        {
            select_room(trigger->room());
            _target = trigger->position();
            _level->set_selected_trigger(trigger->number());
            _triggers_windows->set_selected_trigger(trigger);
            _rooms_windows->set_selected_trigger(trigger);
        }
    }

    void Viewer::select_waypoint(uint32_t index)
    {
        select_room(_route->waypoint(index).room());
        _target = _route->waypoint(index).position();
        _route->select_waypoint(index);
        _route_window_manager->select_waypoint(index);
        if (_settings.auto_orbit)
        {
            set_camera_mode(CameraMode::Orbit);
        }
        _scene_changed = true;
    }

    void Viewer::select_next_waypoint()
    {
        if (_route->selected_waypoint() + 1 < _route->waypoints())
        {
            select_waypoint(_route->selected_waypoint() + 1);
        }
    }

    void Viewer::select_previous_waypoint()
    {
        if (_route->selected_waypoint() > 0)
        {
            select_waypoint(_route->selected_waypoint() - 1);
        }
    }

    void Viewer::remove_waypoint(uint32_t index)
    {
        _route->remove(index);
        _route_window_manager->set_route(_route.get());
        if (_route->waypoints() > 0)
        {
            select_waypoint(_route->selected_waypoint());
        }
        _scene_changed = true;
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
        _scene_target = std::make_unique<graphics::RenderTarget>(_device, static_cast<uint32_t>(size.width), static_cast<uint32_t>(size.height), graphics::RenderTarget::DepthStencilMode::Enabled);
        _scene_sprite->set_host_size(size);
        _scene_changed = true;
    }

    // Set up keyboard and mouse input for the camera.
    void Viewer::setup_camera_input()
    {
        using namespace input;

        _token_store += _mouse.mouse_down += [&](auto button) { _camera_input.mouse_down(button); };
        _token_store += _mouse.mouse_up += [&](auto button) { _camera_input.mouse_up(button); };
        _token_store += _mouse.mouse_move += [&](long x, long y) { _mouse_changed = true; _camera_input.mouse_move(x, y); };
        _token_store += _mouse.mouse_wheel += [&](int16_t scroll) 
        {
            if (window_under_cursor() == _window)
            {
                _ui->set_show_context_menu(false);
                _camera_input.mouse_scroll(scroll);
            }
        };

        _token_store += _camera_input.on_rotate += [&](float x, float y)
        {
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
            // Zoom only affects Orbit mode.
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
            if (_camera_mode != CameraMode::Orbit)
            {
                return;
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

    uint32_t Viewer::room_from_pick(const PickResult& pick) const
    {
        switch (pick.type)
        {
        case PickResult::Type::Room:
            return pick.index;
        case PickResult::Type::Entity:
            return _level->items()[pick.index].room();
        case PickResult::Type::Trigger:
            return _level->triggers()[pick.index]->room();
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
            select_room(pick.index);
            if (pick.override_centre)
            {
                _target = pick.position;
            }
            break;
        case PickResult::Type::Entity:
            select_item(_level->items()[pick.index]);
            break;
        case PickResult::Type::Trigger:
            select_trigger(_level->triggers()[pick.index]);
            break;
        case PickResult::Type::Waypoint:
            select_waypoint(pick.index);
            break;
        }

        if (_settings.auto_orbit)
        {
            set_camera_mode(CameraMode::Orbit);
        }
    }

    void Viewer::register_lua()
    {
        auto state = _lua.state();

        *reinterpret_cast<Viewer**>(lua_newuserdata(state, sizeof(this))) = this;

        luaL_newmetatable(state, "trview.mt");

        lua_pushvalue(state, -1);
        lua_setfield(state, -2, "__index");

        lua_pushcfunction(state, lua_open);
        lua_setfield(state, -2, "open");

        lua_pushcfunction(state, lua_open_recent);
        lua_setfield(state, -2, "openRecent");

        lua_pushcfunction(state, lua_show_tra);
        lua_setfield(state, -2, "tra");

        lua_setmetatable(state, -2);
        lua_setglobal(state, "trview");
    }

    int Viewer::lua_open(lua_State* state)
    {
        auto viewer = (*reinterpret_cast<Viewer**>(luaL_checkudata(state, 1, "trview.mt")));
        auto filename = lua_tostring(state, 2);
        viewer->open(filename);
        return 0;
    }

    int Viewer::lua_open_recent(lua_State* state)
    {
        auto viewer = (*reinterpret_cast<Viewer**>(luaL_checkudata(state, 1, "trview.mt")));
        auto index = static_cast<std::size_t>(lua_tonumber(state, 2));
        const auto& settings = viewer->settings();
        if (index > 0 && index <= settings.recent_files.size())
        {
            viewer->open(*std::next(settings.recent_files.begin(), index - 1));
        }
        return 0;
    }

    int Viewer::lua_show_tra(lua_State* state)
    {
        auto viewer = (*reinterpret_cast<Viewer**>(luaL_checkudata(state, 1, "trview.mt")));
        auto fn = lua_tostring(state, 2);

        auto ox = lua_tonumber(state, 3);
        auto oy = lua_tonumber(state, 4);
        auto oz = lua_tonumber(state, 5);

        using namespace DirectX::SimpleMath;

        // viewer->_route->clear();

        auto drm = lau::load_drm(std::wstring(L"C:\\Projects\\Applications\\trview\\lau\\drm\\") + to_utf16(fn) + L".drm");
        auto count = drm->textures.size();

        float scale = 1.0f / 2048.0f;
        // float scale = 1;
        auto m = Matrix::CreateScale(scale);
        //auto offset = Vector3::Transform(Vector3(drm->world_offset.x, drm->world_offset.z, drm->world_offset.y), m);
        // auto offset = Vector3::Zero;
        // auto offset = Vector3::Transform(Vector3(drm->world_offset.x, -drm->world_offset.z, 0), m);
        // auto offset = Vector3::Transform(Vector3(ox, oy, oz), m);
        auto offset = Vector3(ox, oy, oz);
        // if (std::string(fn) == "gr2")
        // {
        //     offset = Vector3::Zero;
        // }

        float min_x = FLT_MAX;
        float min_y = FLT_MAX;
        float min_z = FLT_MAX;
        float max_x = -FLT_MAX; 
        float max_y = -FLT_MAX;
        float max_z = -FLT_MAX;

        // FILE* stream;
        // AllocConsole();
        // freopen_s(&stream, "CONOUT$", "w", stdout);

        for (const auto& vert : drm->world_mesh)
        {
            min_x = std::min<float>(min_x, vert.x);
            min_y = std::min<float>(min_y, vert.y);
            min_z = std::min<float>(min_z, vert.z);
            max_x = std::max<float>(max_x, vert.x);
            max_y = std::max<float>(max_y, vert.y);
            max_z = std::max<float>(max_z, vert.z);
        }

        // auto extent = Vector3::Transform(Vector3(max_x - min_x, -(max_z - min_z), max_y - min_y), m);
        // auto extent = Vector3::Transform(Vector3(0, max_z - min_z, 0), m);
        // offset += extent * 0.5f;

        for (const auto& vert : drm->world_mesh)
        {
            // viewer->_route->add(Vector3::Transform(Vector3(vert.x, -vert.z, vert.y), m) + offset, 0);
            viewer->_route->add(Vector3::Transform(Vector3(vert.x, vert.y, vert.z) + offset, m), 0);
        }
        
        viewer->_scene_changed = true;
        return 0;
    }
}
