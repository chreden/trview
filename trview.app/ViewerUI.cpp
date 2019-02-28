#include "ViewerUI.h"
#include <trview.ui/Window.h>
#include <trview.graphics/IShaderStorage.h>
#include "ILevelTextureStorage.h"
#include "GoToRoom.h"
#include "ContextMenu.h"

using namespace trview::ui;

namespace trview
{
    ViewerUI::ViewerUI(const Window& window, const graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::FontFactory& font_factory, const ITextureStorage& texture_storage)
        : _mouse(window)
    {
        _control = std::make_unique<ui::Window>(Point(), window.size(), Colour::Transparent);
        _control->set_handles_input(false);

        _token_store += _mouse.mouse_up += [&](auto) { _control->process_mouse_up(client_cursor_position(window)); };
        _token_store += _mouse.mouse_move += [&](auto, auto) { _control->process_mouse_move(client_cursor_position(window)); };
        // Add some extra handlers for the user interface. These will be merged in
        // to one at some point so that the UI can take priority where appropriate.
        _token_store += _mouse.mouse_down += [&](input::Mouse::Button)
        {
            // The client mouse coordinate is already relative to the root window (at present).
            _control->process_mouse_down(client_cursor_position(window));
        };

        generate_tool_window(texture_storage);

        _go_to_room = std::make_unique<GoToRoom>(*_control.get());
        _token_store += _go_to_room->room_selected += [&](uint32_t room) { on_select_room(room); };

        _toolbar = std::make_unique<Toolbar>(*_control);
        _toolbar->add_tool(L"Measure", L"|....|");
        _token_store += _toolbar->on_tool_clicked += [this](const std::wstring& tool)
        {
            if (tool == L"Measure")
            {
                on_tool_selected(Tool::Measure);
            }
        };
        /*
        _context_menu = std::make_unique<ContextMenu>(*_control);
        _token_store += _context_menu->on_add_waypoint += [&]()
        {
            auto type = _context_pick.type == PickResult::Type::Entity ? Waypoint::Type::Entity : _context_pick.type == PickResult::Type::Trigger ? Waypoint::Type::Trigger : Waypoint::Type::Position;
            uint32_t new_index = _route->insert(_context_pick.position, room_from_pick(_context_pick), type, _context_pick.index);
            _context_menu->set_visible(false);
            _route_window_manager->set_route(_route.get());
            select_waypoint(new_index);
        };
        _token_store += _context_menu->on_remove_waypoint += [&]()
        {
            remove_waypoint(_context_pick.index);
            _context_menu->set_visible(false);
        };
        _token_store += _context_menu->on_orbit_here += [&]()
        {
            select_room(room_from_pick(_context_pick));
            _target = _context_pick.position;
            _context_menu->set_visible(false);
        };

        _context_menu->set_remove_enabled(false);
        */

        _level_info = std::make_unique<LevelInfo>(*_control.get(), texture_storage);
        _token_store += _level_info->on_toggle_settings += [&]() { _settings_window->toggle_visibility(); };

        _settings_window = std::make_unique<SettingsWindow>(*_control.get());
        _token_store += _settings_window->on_vsync += [&](bool value)
        {
            // _settings.vsync = value;
            // save_user_settings(_settings);
        };
        _token_store += _settings_window->on_go_to_lara += [&](bool value)
        {
            // _settings.go_to_lara = value;
            // save_user_settings(_settings);
        };
        _token_store += _settings_window->on_invert_map_controls += [&](bool value)
        {
            // _settings.invert_map_controls = value;
            // save_user_settings(_settings);
        };
        _token_store += _settings_window->on_items_startup += [&](bool value)
        {
            // _settings.items_startup = value;
            // save_user_settings(_settings);
        };
        _token_store += _settings_window->on_triggers_startup += [&](bool value)
        {
            // _settings.triggers_startup = value;
            // save_user_settings(_settings);
        };
        _token_store += _settings_window->on_auto_orbit += [&](bool value)
        {
            // _settings.auto_orbit = value;
            // save_user_settings(_settings);
        };
        // _settings_window->set_vsync(_settings.vsync);
        // _settings_window->set_go_to_lara(_settings.go_to_lara);
        // _settings_window->set_invert_map_controls(_settings.invert_map_controls);
        // _settings_window->set_items_startup(_settings.items_startup);
        // _settings_window->set_triggers_startup(_settings.triggers_startup);
        // _settings_window->set_auto_orbit(_settings.auto_orbit);

        // Create the renderer for the UI based on the controls created.
        _ui_renderer = std::make_unique<ui::render::Renderer>(device, shader_storage, font_factory, window.size());
        _ui_renderer->load(_control.get());
        /*
        _map_renderer = std::make_unique<ui::render::MapRenderer>(_device, *_shader_storage.get(), _window.size());
        _token_store += _map_renderer->on_sector_hover += [&](const std::shared_ptr<Sector>& sector)
        {
            if (_level)
            {
                const auto room_info = _current_level->get_room(_level->selected_room()).info;
                _sector_highlight.set_sector(sector,
                    DirectX::SimpleMath::Matrix::CreateTranslation(room_info.x / trlevel::Scale_X, 0, room_info.z / trlevel::Scale_Z));
            }
        };

        _camera_position = std::make_unique<CameraPosition>(*_control);
        */
    }

    void ViewerUI::generate_tool_window(const ITextureStorage& texture_storage)
    {
        // This is the main tool window on the side of the screen.
        auto tool_window = std::make_unique<StackPanel>(Point(), Size(150.0f, 348.0f), Colour(0.5f, 0.0f, 0.0f, 0.0f), Size(5, 5));
        tool_window->set_margin(Size(5, 5));

        _room_navigator = std::make_unique<RoomNavigator>(*tool_window.get(), texture_storage);
        _room_navigator->on_room_selected += on_select_room;
        _room_navigator->on_highlight += on_highlight;
        _room_navigator->on_show_triggers += on_show_triggers;
        _room_navigator->on_show_hidden_geometry += on_show_hidden_geometry;
        _room_navigator->on_show_water += on_show_water;

        _flipmaps = std::make_unique<Flipmaps>(*tool_window.get());
        _flipmaps->on_flip += on_flip;
        _flipmaps->on_alternate_group += on_alternate_group;

        _neighbours = std::make_unique<Neighbours>(*tool_window.get(), texture_storage);
        _neighbours->on_depth_changed += on_depth_level_changed;
        _neighbours->on_enabled_changed += on_depth;

        initialise_camera_controls(*tool_window);

        _control->add_child(std::move(tool_window));
    }

    void ViewerUI::initialise_camera_controls(ui::Control& parent)
    {
        _camera_controls = std::make_unique<CameraControls>(parent);
        // _token_store += _camera_controls->on_reset += [&]() { _camera.reset(); };
        // _token_store += _camera_controls->on_mode_selected += [&](CameraMode mode) { set_camera_mode(mode); };
        // _token_store += _camera_controls->on_sensitivity_changed += [&](float value) { _settings.camera_sensitivity = value; };
        // _token_store += _camera_controls->on_movement_speed_changed += [&](float value) { _settings.camera_movement_speed = value; };

        // _camera_controls->set_sensitivity(_settings.camera_sensitivity);
        // _camera_controls->set_mode(CameraMode::Orbit);
        // _camera_controls->set_movement_speed(_settings.camera_movement_speed == 0 ? _CAMERA_MOVEMENT_SPEED_DEFAULT : _settings.camera_movement_speed);
    }

    void ViewerUI::render(const graphics::Device& device)
    {
        _ui_renderer->render(device.context());
    }

    void ViewerUI::set_alternate_group(uint16_t value, bool enabled)
    {
        _flipmaps->set_alternate_group(value, enabled);
    }

    void ViewerUI::set_alternate_groups(const std::set<uint16_t>& groups)
    {
        _flipmaps->set_alternate_groups(groups);
    }

    void ViewerUI::set_depth_enabled(bool value)
    {
        _neighbours->set_enabled(value);
    }

    void ViewerUI::set_depth_level(int32_t value)
    {
        _neighbours->set_depth(value);
    }

    void ViewerUI::set_flip(bool value)
    {
        _flipmaps->set_flip(value);
    }

    void ViewerUI::set_flip_enabled(bool value)
    {
        _flipmaps->set_flip_enabled(value);
    }

    void ViewerUI::set_highlight(bool value)
    {
        _room_navigator->set_highlight(value);
    }

    void ViewerUI::set_show_hidden_geometry(bool value)
    {
        _room_navigator->set_show_hidden_geometry(value);
    }

    void ViewerUI::set_show_triggers(bool value)
    {
        _room_navigator->set_show_triggers(value);
    }

    void ViewerUI::set_show_water(bool value)
    {
        _room_navigator->set_show_water(value);
    }

    void ViewerUI::set_use_alternate_groups(bool value)
    {
        _flipmaps->set_use_alternate_groups(value);
    }

    void ViewerUI::set_visible(bool value)
    {
        _control->set_visible(value);
    }

    bool ViewerUI::show_hidden_geometry() const
    {
        return _room_navigator->show_hidden_geometry();
    }

    bool ViewerUI::show_triggers() const
    {
        return _room_navigator->show_triggers();
    }

    bool ViewerUI::show_water() const
    {
        return _room_navigator->show_water();
    }
}
