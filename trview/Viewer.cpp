#include "stdafx.h"
#include "Viewer.h"

#include <algorithm>
#include <string>
#include <directxmath.h>

#include <trlevel/trlevel.h>
#include <trview.graphics/ShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include <trview.graphics/DeviceWindow.h>
#include <trview.ui/Control.h>
#include <trview.ui/StackPanel.h>
#include <trview.ui/Window.h>
#include <trview.ui/Label.h>
#include <trview.ui.render/Renderer.h>
#include <trview.ui.render/MapRenderer.h>

#include "CameraControls.h"
#include "DefaultTextures.h"
#include "DefaultShaders.h"
#include "DefaultFonts.h"
#include "GoToRoom.h"
#include "LevelInfo.h"
#include "Neighbours.h"
#include "RoomNavigator.h"
#include "TextureStorage.h"
#include "SettingsWindow.h"

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
        _window_resizer(window), _recent_files(window), _file_dropper(window)
    {
        _settings = load_user_settings();

        _shader_storage = std::make_unique<graphics::ShaderStorage>();
        load_default_shaders(_device.device(), *_shader_storage.get());

        _font_factory = std::make_unique<graphics::FontFactory>(_device.device());
        load_default_fonts(_device.device(), *_font_factory.get());

        _main_window = _device.create_for_window(window);
        _items_windows = std::make_unique<ItemsWindowManager>(_device, *_shader_storage.get(), *_font_factory.get(), window);
        if (_settings.items_startup)
        {
            _items_windows->create_window();
        }
        _token_store.add(_items_windows->on_item_selected += [this](const auto& item) { select_item(item); });
        _token_store.add(_items_windows->on_trigger_selected += [this](const auto& trigger) { select_trigger(trigger); });

        _triggers_windows = std::make_unique<TriggersWindowManager>(_device, *_shader_storage.get(), *_font_factory.get(), window);
        if (_settings.triggers_startup)
        {
            _triggers_windows->create_window();
        }
        _token_store.add(_triggers_windows->on_item_selected += [this](const auto& item) { select_item(item); });
        _token_store.add(_triggers_windows->on_trigger_selected += [this](const auto& trigger) { select_trigger(trigger); });

        _token_store.add(_level_switcher.on_switch_level += [=](const auto& file) { open(file); });
        _token_store.add(on_file_loaded += [&](const auto& file) { _level_switcher.open_file(file); });

        _token_store.add(_window_resizer.on_resize += [=]()
        {
            _main_window->resize();
            resize_elements();
        });

        _token_store.add(_recent_files.on_file_open += [=](const auto& file) { open(file); });
        _recent_files.set_recent_files(_settings.recent_files);
        _token_store.add(on_recent_files_changed += [&](const auto& files) { _recent_files.set_recent_files(files); });

        _token_store.add(_file_dropper.on_file_dropped += [&](const auto& file) { open(file); });

        initialise_input();

        _texture_storage = std::make_unique<TextureStorage>(_device.device());
        load_default_textures(_device.device(), *_texture_storage.get());

        generate_ui();
    }

    Viewer::~Viewer()
    {
        save_user_settings(_settings);
    }

    UserSettings Viewer::settings() const
    {
        return _settings;
    }

    void Viewer::generate_ui()
    {
        // Create the user interface window. At the moment this is going to be a bar on the side, 
        // but this can change over time. For now make a really boring gray window.
        _control = std::make_unique<ui::Window>(Point(), _window.size(), Colour(0.f, 0.f, 0.f, 0.f)); 
        _control->set_handles_input(false);

        generate_tool_window();

        _go_to_room = std::make_unique<GoToRoom>(*_control.get());
        _token_store.add(_go_to_room->room_selected += [&](uint32_t room)
        {
            select_room(room);
        });

        auto picking = std::make_unique<ui::Label>(Point(500, 0), Size(38, 30), Colour(0.2f, 0.2f, 0.2f), L"0", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre);
        picking->set_visible(false);
        picking->set_handles_input(false);
        _picking = _control->add_child(std::move(picking));

        _toolbar = std::make_unique<Toolbar>(*_control);

        auto measure_label = std::make_unique<ui::Label>(Point(300, 100), Size(50, 30), Colour(1, 0.5f, 0.5f, 0.5f), L"0", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre);
        _measure_label = _control->add_child(std::move(measure_label));

        _level_info = std::make_unique<LevelInfo>(*_control.get(), *_texture_storage.get());
        _token_store.add(_level_info->on_toggle_settings += [&]() { _settings_window->toggle_visibility(); });

        _settings_window = std::make_unique<SettingsWindow>(*_control.get());
        _token_store.add(_settings_window->on_vsync += [&](bool value)
        { 
            _settings.vsync = value; 
            save_user_settings(_settings);
        });
        _token_store.add(_settings_window->on_go_to_lara += [&](bool value)
        { 
            _settings.go_to_lara = value; 
            save_user_settings(_settings); 
        });
        _token_store.add(_settings_window->on_invert_map_controls += [&](bool value)
        {
            _settings.invert_map_controls = value;
            save_user_settings(_settings);
        });
        _token_store.add(_settings_window->on_items_startup += [&](bool value)
        {
            _settings.items_startup = value;
            save_user_settings(_settings);
        });
        _token_store.add(_settings_window->on_triggers_startup += [&](bool value)
        {
            _settings.triggers_startup = value;
            save_user_settings(_settings);
        });
        _settings_window->set_vsync(_settings.vsync);
        _settings_window->set_go_to_lara(_settings.go_to_lara);
        _settings_window->set_invert_map_controls(_settings.invert_map_controls);
        _settings_window->set_items_startup(_settings.items_startup);
        _settings_window->set_triggers_startup(_settings.triggers_startup);

        // Create the renderer for the UI based on the controls created.
        _ui_renderer = std::make_unique<ui::render::Renderer>(_device.device(), *_shader_storage.get(), *_font_factory.get(), _window.size());
        _ui_renderer->load(_control.get());

        _map_renderer = std::make_unique<ui::render::MapRenderer>(_device.device(), *_shader_storage.get(), _window.size());
    }

    void Viewer::generate_tool_window()
    {
        using namespace ui;

        // This is the main tool window on the side of the screen.
        auto tool_window = std::make_unique<ui::StackPanel>(Point(), Size(150.0f, 348.0f), Colour(1.f, 0.5f, 0.5f, 0.5f), Size(5, 5));

        _room_navigator = std::make_unique<RoomNavigator>(*tool_window.get(), *_texture_storage.get());
        _token_store.add(_room_navigator->on_room_selected += [&](uint32_t room) { select_room(room); });
        _token_store.add(_room_navigator->on_highlight += [&](bool) { toggle_highlight(); });
        _token_store.add(_room_navigator->on_flip += [&](bool flip) { set_alternate_mode(flip); });
        _token_store.add(_room_navigator->on_show_triggers += [&](bool show) { set_show_triggers(show); });

        _neighbours = std::make_unique<Neighbours>(*tool_window.get(), *_texture_storage.get());
        _token_store.add(_neighbours->on_depth_changed += [&](int32_t value)
        {
            if (_level)
            {
                _level->set_neighbour_depth(value);
            }
        });
        _token_store.add(_neighbours->on_enabled_changed += [&](bool enabled)
        {
            if (_level)
            {
                _level->set_highlight_mode(enabled ? Level::RoomHighlightMode::Neighbours : Level::RoomHighlightMode::None);
                _room_navigator->set_highlight(false);
            }
        });

        _camera_controls = std::make_unique<CameraControls>(*tool_window.get());
        _token_store.add(_camera_controls->on_reset += [&]() { _camera.reset(); });
        _token_store.add(_camera_controls->on_mode_selected += [&](CameraMode mode) { set_camera_mode(mode); });
        _token_store.add(_camera_controls->on_sensitivity_changed += [&](float value)
        {
            _settings.camera_sensitivity = value;
        });

        _token_store.add(_camera_controls->on_movement_speed_changed += [&](float value)
        {
            _settings.camera_movement_speed = value;
        });

        _camera_controls->set_sensitivity(_settings.camera_sensitivity);
        _camera_controls->set_mode(CameraMode::Orbit);

        _camera_controls->set_movement_speed (
            _settings.camera_movement_speed == 0? _CAMERA_MOVEMENT_SPEED_DEFAULT: _settings.camera_movement_speed
        );

        _control->add_child(std::move(tool_window));
    }

    void Viewer::initialise_input()
    {
        _token_store.add(_keyboard.on_key_up += std::bind(&Viewer::process_input_key, this, std::placeholders::_1));
        _token_store.add(_keyboard.on_char += std::bind(&Viewer::process_char, this, std::placeholders::_1));

        _token_store.add(_keyboard.on_key_down += [&](auto key) {_camera_input.key_down(key); });
        _token_store.add(_keyboard.on_key_up += [&](auto key) {_camera_input.key_up(key); });

        _token_store.add(_keyboard.on_key_down += [&](uint16_t key)
        {
            if (GetAsyncKeyState(VK_CONTROL))
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
                case 'T':
                {
                    if (_level)
                    {
                        set_show_triggers(!_level->show_triggers());
                    }
                    break;
                }
            }
        });

        setup_camera_input();

        using namespace input;

        _token_store.add(_mouse.mouse_down += [&](Mouse::Button button)
        {
            if (button == Mouse::Button::Left)
            {
                if (!over_ui() && !over_map() && _picking->visible() && _current_pick.hit)
                {
                    _previous_pick = _current_pick;

                    if (_current_pick.type == PickResult::Type::Room)
                    {
                        select_room(_current_pick.index);
                    }
                    else if (_current_pick.type == PickResult::Type::Entity)
                    {
                        select_item(_level->items()[_current_pick.index]);
                    }
                    else if (_current_pick.type == PickResult::Type::Trigger)
                    {
                        select_trigger(_level->triggers()[_current_pick.index]);
                    }
                    set_camera_mode(CameraMode::Orbit);
                }
                else if (over_map())
                {
                    std::shared_ptr<Sector> sector = _map_renderer->sector_at_cursor(); 
                    if (sector)
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
            }
            else if (button == Mouse::Button::Right)
            {
                if (over_map())
                {
                    std::shared_ptr<Sector> sector = _map_renderer->sector_at_cursor(); 
                    if (sector) {
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
            }
        });

        _token_store.add(_mouse.mouse_up += [&](auto) { _control->process_mouse_up(client_cursor_position(_window)); });
        _token_store.add(_mouse.mouse_move += [&](auto, auto) { _control->process_mouse_move(client_cursor_position(_window)); });

        // Add some extra handlers for the user interface. These will be merged in
        // to one at some point so that the UI can take priority where appropriate.
        _token_store.add(_mouse.mouse_down += [&](Mouse::Button)
        {
            // The client mouse coordinate is already relative to the root window (at present).
            _control->process_mouse_down(client_cursor_position(_window));
        });
    }

    void Viewer::process_input_key(uint16_t key)
    {
        if (_go_to_room->visible())
        {
            if (key == 'R' && _keyboard.control())
            {
                _go_to_room->toggle_visible();
            }
            else
            {
                _go_to_room->input(key);
            }
        }
        else
        {
            switch (key)
            {
                case 'R':
                {
                    if (_keyboard.control())
                    {
                        _go_to_room->toggle_visible();
                    }
                    break;
                }
                case VK_F1:
                    _settings_window->toggle_visibility();
                    break;
                case VK_RETURN:
                    toggle_highlight();
                    break;
                case VK_INSERT:
                {
                    // Reset the camera to defaults.
                    _camera.set_rotation_yaw(0.f);
                    _camera.set_rotation_pitch(0.78539f);
                    _camera.set_zoom(8.f);
                    break;
                }
            }
        }
    }

    void Viewer::process_char(uint16_t character)
    {
        if (_go_to_room->visible())
        {
            _go_to_room->character(character);
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
    }

    void Viewer::open(const std::wstring filename)
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

        _level = std::make_unique<Level>(_device.device(), *_shader_storage.get(), _current_level.get());
        _token_store.add(_level->on_room_selected += [&](uint16_t room) { select_room(room); });
        _token_store.add(_level->on_alternate_mode_selected += [&](bool enabled) { set_alternate_mode(enabled); });

        _items_windows->set_items(_level->items());
        _items_windows->set_triggers(_level->triggers());
        _triggers_windows->set_items(_level->items());
        _triggers_windows->set_triggers(_level->triggers());

        _level->set_show_triggers(_room_navigator->show_triggers());

        // Set up the views.
        auto rooms = _level->room_info();
        _camera.reset();

        // Reset UI buttons
        _room_navigator->set_max_rooms(static_cast<uint32_t>(rooms.size()));
        _room_navigator->set_highlight(false);
        _room_navigator->set_flip(false);
        _room_navigator->set_flip_enabled(_level->any_alternates());

        trlevel::tr2_entity lara_entity;
        if (_settings.go_to_lara && _current_level->find_first_entity_by_type(0, lara_entity))
        {
            select_room(lara_entity.Room);
        }
        else
        {
            select_room(0);
        }

        _neighbours->set_enabled(false);
        _neighbours->set_depth(1);

        // Strip the last part of the path away.
        auto last_index = std::min(filename.find_last_of('\\'), filename.find_last_of('/'));
        auto name = last_index == filename.npos ? filename : filename.substr(std::min(last_index + 1, filename.size()));
        _level_info->set_level(name);
        _level_info->set_level_version(_current_level->get_version());
    }

    void Viewer::render()
    {
        _timer.update();

        update_camera();

        pick();

        _device.begin();
        _main_window->begin();
        _main_window->clear(DirectX::SimpleMath::Color(0.0f, 0.2f, 0.4f, 1.0f));

        render_scene();
        _ui_renderer->render(_device.context());
        render_map();

        _main_window->present(_settings.vsync);

        _items_windows->render(_device, _settings.vsync);
        _triggers_windows->render(_device, _settings.vsync);
    }

    // Determines whether the cursor is over a UI element that would take any input.
    // Returns: True if there is any UI under the cursor that would take input.
    bool Viewer::over_ui() const
    {
        return _control->is_mouse_over(client_cursor_position(_window));
    }

    bool Viewer::over_map() const 
    {
        return _map_renderer->loaded() && _map_renderer->cursor_is_over_control();
    }

    void Viewer::pick()
    {
        if (!_level || window_under_cursor() != _window || window_is_minimised(_window) || over_ui() || over_map() || cursor_outside_window(_window))
        {
            _picking->set_visible(false);
            return;
        }

        using namespace DirectX;
        using namespace SimpleMath;

        const ICamera& camera = current_camera();
        const Vector3 position = camera.position();
        auto world = Matrix::CreateTranslation(position);
        auto projection = camera.projection();
        auto view = camera.view();

        Point mouse_pos = client_cursor_position(_window);
        const auto window_size = _window.size();

        Vector3 direction = XMVector3Unproject(Vector3(mouse_pos.x, mouse_pos.y, 1), 0, 0, window_size.width, window_size.height, 0, 1.0f, projection, view, world);
        direction.Normalize();

        auto result = _level->pick(camera, position, direction);

        _picking->set_visible(result.hit);
        if (result.hit)
        {
            _measure_label->set_text(std::to_wstring((_current_pick.position - _previous_pick.position).Length()));

            Vector3 screen_pos = XMVector3Project(result.position, 0, 0, window_size.width, window_size.height, 0, 1.0f, projection, view, XMMatrixIdentity());
            _picking->set_position(Point(screen_pos.x - _picking->size().width, screen_pos.y - _picking->size().height));
            _picking->set_text((result.type == PickResult::Type::Room ? L"R-" : result.type == PickResult::Type::Trigger ? L"T-" : L"I-") + std::to_wstring(result.index));
            _picking->set_text_colour(result.type == PickResult::Type::Room ? Colour(1.0f, 1.0f, 1.0f) : result.type == PickResult::Type::Trigger ? Colour(1.0f, 0.0f, 1.0f) : Colour(0.0f, 1.0f, 0.0f));
        }
        _current_pick = result;
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
            _level->render(_device.context(), current_camera());
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
        _camera_controls->set_mode(camera_mode);
    }

    void Viewer::render_map()
    {
        Point point = client_cursor_position(_window);
        _map_renderer->set_cursor_position(point);
        _map_renderer->render(_device.context());
    }

    void Viewer::toggle_highlight()
    {
        if (_level)
        {
            if (_level->highlight_mode() == Level::RoomHighlightMode::Highlight)
            {
                _level->set_highlight_mode(Level::RoomHighlightMode::None);
                _room_navigator->set_highlight(false);
                _neighbours->set_enabled(false);
            }
            else
            {
                _level->set_highlight_mode(Level::RoomHighlightMode::Highlight);
                _room_navigator->set_highlight(true);
                _neighbours->set_enabled(false);
            }
        }
    }

    void Viewer::select_room(uint32_t room)
    {
        if (_current_level && room < _current_level->num_rooms())
        {
            _level->set_selected_room(static_cast<uint16_t>(room));

            _room_navigator->set_selected_room(room);
            _room_navigator->set_room_info(_level->room_info(room));

            _map_renderer->load(_level->room(_level->selected_room()));

            set_camera_mode(CameraMode::Orbit);

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
            _target = DirectX::SimpleMath::Vector3(entity.x / 1024.0f, entity.y / -1024.0f, entity.z / 1024.0f);
            _level->set_selected_item(item.number());
            _items_windows->set_selected_item(item);
        }
    }

    void Viewer::select_trigger(const Trigger* const trigger)
    {
        if (_level)
        {
            select_room(trigger->room());

            const auto room = _level->room(trigger->room());
            const auto room_info = room->info();

            // Calculate the X/Z position - the Y must be determined by casting a ray from above 
            // directly down, to see what it hits. If it hits nothing, use the centre of the room.
            const float x = room_info.x / 1024.0f + trigger->x() + 0.5f;
            const float z = room_info.z / 1024.0f + (room->num_z_sectors() - 1 - trigger->z()) + 0.5f;

            using namespace DirectX::SimpleMath;
            const auto pick = room->pick(Vector3(x, 500.0f, z), Vector3(0, -1, 0), false);
            const float y = pick.hit ? pick.position.y : room->centre().y;

            _target = DirectX::SimpleMath::Vector3(x, y, z);

            _level->set_selected_trigger(trigger->number());
            _triggers_windows->set_selected_trigger(trigger);
        }
    }

    void Viewer::set_alternate_mode(bool enabled)
    {
        if (_level)
        {
            _level->set_alternate_mode(enabled);
            _room_navigator->set_flip(enabled);
        }
    }

    void Viewer::resize_elements()
    {
        const auto size = _window.size();
        // Inform elements that need to know that the device has been resized.
        _camera.set_view_size(size);
        _free_camera.set_view_size(size);
        _control->set_size(size);
        _ui_renderer->set_host_size(size);
        _map_renderer->set_window_size(size);
    }

    // Set up keyboard and mouse input for the camera.
    void Viewer::setup_camera_input()
    {
        using namespace input;

        _token_store.add(_mouse.mouse_down += [&](auto button) { _camera_input.mouse_down(button); });
        _token_store.add(_mouse.mouse_up += [&](auto button) { _camera_input.mouse_up(button); });
        _token_store.add(_mouse.mouse_move += [&](long x, long y) { _camera_input.mouse_move(x, y); });
        _token_store.add(_mouse.mouse_wheel += [&](int16_t scroll) 
        {
            if (window_under_cursor() == _window)
            {
                _camera_input.mouse_scroll(scroll);
            }
        });

        _token_store.add(_camera_input.on_rotate += [&](float x, float y)
        {
            ICamera& camera = current_camera();
            const float low_sensitivity = 200.0f;
            const float high_sensitivity = 25.0f;
            const float sensitivity = low_sensitivity + (high_sensitivity - low_sensitivity) * _settings.camera_sensitivity;
            camera.set_rotation_yaw(camera.rotation_yaw() + x / sensitivity);
            camera.set_rotation_pitch(camera.rotation_pitch() + y / sensitivity);
            if (_level)
            {
                _level->on_camera_moved();
            }
        });

        _token_store.add(_camera_input.on_zoom += [&](float zoom)
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
        });

        _token_store.add(_camera_input.on_mode_change += [&](CameraMode mode) { set_camera_mode(mode); });
    }

    void Viewer::set_show_triggers(bool show)
    {
        if (_level)
        {
            _level->set_show_triggers(show);
            _room_navigator->set_show_triggers(show);
        }
    }
}
