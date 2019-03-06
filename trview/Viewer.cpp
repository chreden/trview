#include "stdafx.h"
#include "Viewer.h"

#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <directxmath.h>

#include <trlevel/trlevel.h>
#include <trview.graphics/ShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include <trview.graphics/DeviceWindow.h>

#include "DefaultTextures.h"
#include "DefaultShaders.h"
#include "DefaultFonts.h"
#include "TextureStorage.h"

namespace trview
{
    namespace
    {
        const float _CAMERA_MOVEMENT_SPEED_MULTIPLIER = 23.0f;
        const float _CAMERA_MOVEMENT_SPEED_DEFAULT = 0.5f;
    }

    Viewer::Viewer(const Window& window)
        : _window(window), _camera(window.size()), _free_camera(window.size()),
        _timer(default_time_source()), _keyboard(window), _mouse(window), _level_switcher(window),
        _window_resizer(window), _recent_files(window), _file_dropper(window), _alternate_group_toggler(window),
        _view_menu(window)
    {
        _settings = load_user_settings();

        _shader_storage = std::make_unique<graphics::ShaderStorage>();
        load_default_shaders(_device, *_shader_storage.get());

        load_default_fonts(_device, _font_factory);

        _main_window = _device.create_for_window(window);
        _items_windows = std::make_unique<ItemsWindowManager>(_device, *_shader_storage.get(), _font_factory, window);
        if (_settings.items_startup)
        {
            _items_windows->create_window();
        }
        _token_store += _items_windows->on_item_selected += [this](const auto& item) { select_item(item); };
        _token_store += _items_windows->on_trigger_selected += [this](const auto& trigger) { select_trigger(trigger); };
        _token_store += _items_windows->on_add_to_route += [this](const auto& item)
        {
            auto entity = _current_level->get_entity(item.number());
            uint32_t new_index = _route->insert(entity.position(), item.room(), Waypoint::Type::Entity, item.number());
            _route_window_manager->set_route(_route.get());
            select_waypoint(new_index);
        };

        _triggers_windows = std::make_unique<TriggersWindowManager>(_device, *_shader_storage.get(), _font_factory, window);
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
            if (!_ui->go_to_room_visible())
            {
                set_alternate_group(group, !alternate_group(group));
            }
        };

        initialise_input();

        _texture_storage = std::make_unique<TextureStorage>(_device);
        load_default_textures(_device, *_texture_storage.get());

        _ui = std::make_unique<ViewerUI>(_window, _device, *_shader_storage, _font_factory, *_texture_storage);
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
        _token_store += _ui->on_alternate_group += [&](uint16_t group, bool value) { set_alternate_group(group, value); };
        _token_store += _ui->on_depth += [&](bool value) { if (_level) { _level->set_highlight_mode(Level::RoomHighlightMode::Neighbours, value); } };
        _token_store += _ui->on_depth_level_changed += [&](int32_t value) { if (_level) { _level->set_neighbour_depth(value); } };
        _token_store += _ui->on_camera_reset += [&]() { _camera.reset(); };
        _token_store += _ui->on_camera_mode += [&](CameraMode mode) { set_camera_mode(mode); };
        _token_store += _ui->on_camera_sensitivity += [&](float value) { _settings.camera_sensitivity = value; };
        _token_store += _ui->on_camera_movement_speed += [&](float value) { _settings.camera_movement_speed = value; };
        _token_store += _ui->on_sector_hover += [&](const std::shared_ptr<Sector>& sector)
            {
                if (_level)
                {
                    const auto room_info = _current_level->get_room(_level->selected_room()).info;
                    _sector_highlight.set_sector(sector,
                        DirectX::SimpleMath::Matrix::CreateTranslation(room_info.x / trlevel::Scale_X, 0, room_info.z / trlevel::Scale_Z));
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
            select_room(room_from_pick(_context_pick));
            _target = _context_pick.position;
        };
        _token_store += _ui->on_settings += [&](auto settings) { _settings = settings; };

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

        _route_window_manager = std::make_unique<RouteWindowManager>(_device, *_shader_storage, _font_factory, window);
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

        _token_store += _view_menu.on_show_minimap += [&](bool show) { _ui->set_show_minimap(show); };
        _token_store += _view_menu.on_show_tooltip += [&](bool show) { _ui->set_show_tooltip(show); };
        _token_store += _view_menu.on_show_ui += [&](bool show) { _ui->set_visible(show); };
        _token_store += _view_menu.on_show_compass += [&](bool show) { _compass->set_visible(show); };
        _token_store += _view_menu.on_show_selection += [&](bool show) { _show_selection = show; };
        _token_store += _view_menu.on_show_route += [&](bool show) { _show_route = show; };
        _token_store += _view_menu.on_show_tools += [&](bool show) { _measure->set_visible(show); };

        _picking = std::make_unique<Picking>();
        _token_store += _picking->pick_sources += [&](PickInfo info, PickResult& result) { result.stop = !should_pick(); };
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
        _token_store += _picking->pick_sources += [&](PickInfo info, PickResult& result)
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
                    result.text = L"|....|";
                }
            }
        };

        _token_store += _picking->on_pick += [&](const PickInfo& info, const PickResult& result)
        {
            _current_pick = result;

            _ui->set_pick(info, result);

            // Highlight sectors in the minimap.
            if (_level)
            {
                std::optional<RoomInfo> info;
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

                if (!info.has_value())
                {
                    _ui->clear_minimap_highlight();
                    return;
                }

                auto x = _current_pick.position.x - (info.value().x / trlevel::Scale_X);
                auto z = _current_pick.position.z - (info.value().z / trlevel::Scale_Z);
                _ui->set_minimap_highlight(x, z);
            }
        };
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
        _token_store += _keyboard.on_key_up += std::bind(&Viewer::process_input_key, this, std::placeholders::_1);

        _token_store += _keyboard.on_key_down += [&](auto key) {_camera_input.key_down(key); };
        _token_store += _keyboard.on_key_up += [&](auto key) {_camera_input.key_up(key); };

        _token_store += _keyboard.on_key_down += [&](uint16_t key)
        {
            if (_ui->go_to_room_visible() || GetAsyncKeyState(VK_CONTROL))
            {
                return;
            }

            switch (key)
            {
                case 'P':
                {
                    if (_level && _level->any_alternates())
                    {
                        set_alternate_mode(!_level->alternate_mode());
                    }
                    break;
                }
                case 'M':
                {
                    _active_tool = Tool::Measure;
                    _measure->reset();
                    break;
                }
                case 'T':
                {
                    if (_level)
                    {
                        set_show_triggers(!_level->show_triggers());
                    }
                    break;
                }
                case VK_LEFT:
                {
                    if (_route->selected_waypoint() > 0)
                    {
                        select_waypoint(_route->selected_waypoint() - 1);
                    }
                    break;
                }
                case VK_RIGHT:
                {
                    if (_route->selected_waypoint() + 1 < _route->waypoints())
                    {
                        select_waypoint(_route->selected_waypoint() + 1);
                    }
                    break;
                }
                case VK_DELETE:
                {
                    remove_waypoint(_route->selected_waypoint());
                    break;
                }
            }
        };

        setup_camera_input();

        using namespace input;

        _token_store += _mouse.mouse_down += [&](Mouse::Button button)
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
                            switch (_current_pick.type)
                            {
                            case PickResult::Type::Room:
                                select_room(_current_pick.index);
                                break;
                            case PickResult::Type::Entity:
                                select_item(_level->items()[_current_pick.index]);
                                break;
                            case PickResult::Type::Trigger:
                                select_trigger(_level->triggers()[_current_pick.index]);
                                break;
                            case PickResult::Type::Waypoint:
                                select_waypoint(_current_pick.index);
                                break;
                            }

                            if (_settings.auto_orbit)
                            {
                                set_camera_mode(CameraMode::Orbit);
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

                    if (trigger == triggers.end() || GetAsyncKeyState(VK_CONTROL))
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

    void Viewer::process_input_key(uint16_t key)
    {
        if (!_ui->go_to_room_visible())
        {
            switch (key)
            {
                case 'G':
                {
                    if(_level && !_keyboard.control())
                    {
                        set_show_hidden_geometry(!_level->show_hidden_geometry());
                    }
                    break;
                }
                case VK_F1:
                    _ui->toggle_settings_visibility();
                    break;
                case 'H':
                    toggle_highlight();
                    break;
                case VK_INSERT:
                {
                    // Reset the camera to defaults.
                    _camera.set_rotation_yaw(0.f);
                    _camera.set_rotation_pitch(-0.78539f);
                    _camera.set_zoom(8.f);
                    break;
                }
            }
        }
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

        _ui->set_camera_position(current_camera().position());
    }

    void Viewer::open(const std::string& filename)
    {
        try
        {
            _current_level = trlevel::load_level(filename);
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

        _level = std::make_unique<Level>(_device, *_shader_storage.get(), _current_level.get());
        _token_store += _level->on_room_selected += [&](uint16_t room) { select_room(room); };
        _token_store += _level->on_alternate_mode_selected += [&](bool enabled) { set_alternate_mode(enabled); };
        _token_store += _level->on_alternate_group_selected += [&](uint16_t group, bool enabled) { set_alternate_group(group, enabled); };

        _items_windows->set_items(_level->items());
        _items_windows->set_triggers(_level->triggers());
        _triggers_windows->set_items(_level->items());
        _triggers_windows->set_triggers(_level->triggers());
        _route_window_manager->set_items(_level->items());
        _route_window_manager->set_triggers(_level->triggers());

        _level->set_show_triggers(_ui->show_triggers());
        _level->set_show_hidden_geometry(_ui->show_hidden_geometry());
        _level->set_show_water(_ui->show_water());

        // Set up the views.
        auto rooms = _level->room_info();
        _camera.reset();

        // Reset UI buttons
        _ui->set_max_rooms(static_cast<uint32_t>(rooms.size()));
        _ui->set_highlight(false);
        _ui->set_use_alternate_groups(_current_level->get_version() >= trlevel::LevelVersion::Tomb4);
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
        _ui->set_level(name, _current_level->get_version());
        _window.set_title("trview - " + name);
        _measure->reset();
        _route->clear();
        _route_window_manager->set_route(_route.get());
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

        _picking->pick(_window, current_camera());

        _device.begin();
        _main_window->begin();
        _main_window->clear(DirectX::SimpleMath::Color(0.0f, 0.2f, 0.4f, 1.0f));

        render_scene();
        _ui->render(_device);

        _main_window->present(_settings.vsync);

        _items_windows->render(_device, _settings.vsync);
        _triggers_windows->render(_device, _settings.vsync);
        _route_window_manager->render(_device, _settings.vsync);
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
            if (_current_level->num_rooms() > 0)
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

    void Viewer::select_room(uint32_t room)
    {
        if (_current_level && room < _current_level->num_rooms())
        {
            _level->set_selected_room(static_cast<uint16_t>(room));
            _ui->set_selected_room(_level->room(_level->selected_room()));

            if (_settings.auto_orbit)
            {
                set_camera_mode(CameraMode::Orbit);
            }

            _target = _level->room(_level->selected_room())->centre();

            _items_windows->set_room(room);
            _triggers_windows->set_room(room);
        }
    }

    void Viewer::select_item(const Item& item)
    {
        if (_current_level && item.number() < _current_level->num_entities())
        {
            select_room(item.room());
            auto entity = _current_level->get_entity(item.number());
            _target = entity.position();
            _level->set_selected_item(item.number());
            _items_windows->set_selected_item(item);
        }
    }

    void Viewer::select_trigger(const Trigger* const trigger)
    {
        if (_level)
        {
            select_room(trigger->room());
            _target = trigger->position();
            _level->set_selected_trigger(trigger->number());
            _triggers_windows->set_selected_trigger(trigger);
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
    }

    void Viewer::remove_waypoint(uint32_t index)
    {
        _route->remove(index);
        _route_window_manager->set_route(_route.get());
        if (_route->waypoints() > 0)
        {
            select_waypoint(_route->selected_waypoint());
        }
    }

    void Viewer::set_alternate_mode(bool enabled)
    {
        if (_level)
        {
            _level->set_alternate_mode(enabled);
            _ui->set_flip(enabled);
        }
    }

    void Viewer::set_alternate_group(uint16_t group, bool enabled)
    {
        if (_level)
        {
            _level->set_alternate_group(group, enabled);
            _ui->set_alternate_group(group, enabled);
        }
    }

    bool Viewer::alternate_group(uint16_t group) const
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
    }

    // Set up keyboard and mouse input for the camera.
    void Viewer::setup_camera_input()
    {
        using namespace input;

        _token_store += _mouse.mouse_down += [&](auto button) { _camera_input.mouse_down(button); };
        _token_store += _mouse.mouse_up += [&](auto button) { _camera_input.mouse_up(button); };
        _token_store += _mouse.mouse_move += [&](long x, long y) { _camera_input.mouse_move(x, y); };
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

    void Viewer::set_show_hidden_geometry(bool show)
    {
        if (_level)
        {
            _level->set_show_hidden_geometry(show);
            _ui->set_show_hidden_geometry(show);
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
}
