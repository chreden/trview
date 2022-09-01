#include "ViewerUI.h"
#include <trview.app/Graphics/ILevelTextureStorage.h>
#include "GoTo.h"
#include <trview.app/UI/ContextMenu.h>
#include <trview.input/WindowTester.h>
#include <trview.common/Windows/Shortcuts.h>

namespace trview
{
    ViewerUI::ViewerUI(const Window& window, const std::shared_ptr<ITextureStorage>& texture_storage,
        const std::shared_ptr<IShortcuts>& shortcuts,
        const IMapRenderer::Source& map_renderer_source,
        std::unique_ptr<ISettingsWindow> settings_window,
        std::unique_ptr<IViewOptions> view_options,
        std::unique_ptr<IContextMenu> context_menu,
        std::unique_ptr<ICameraControls> camera_controls)
        : _mouse(window, std::make_unique<input::WindowTester>(window)), _window(window), _camera_controls(std::move(camera_controls)),
        _view_options(std::move(view_options)), _settings_window(std::move(settings_window)), _context_menu(std::move(context_menu))
    {
        _token_store += _mouse.mouse_move += [&](long, long)
        {
            _map_renderer->set_cursor_position(client_cursor_position(_window));
        };

        _token_store += shortcuts->add_shortcut(true, 'G') += [&]()
        {
            if (!is_input_active())
            {
                _go_to->set_name("Room");
                _go_to->toggle_visible(_selected_room);
            }
        };

        _token_store += shortcuts->add_shortcut(true, 'E') += [&]()
        {
            if (!is_input_active())
            {
                _go_to->set_name("Item");
                _go_to->toggle_visible(_selected_item);
            }
        };

        _token_store += shortcuts->add_shortcut(false, VK_F11) += [&]()
        {
            if (!is_input_active())
            {
                _console->set_visible(!_console->visible());
            }
        };

        generate_tool_window();

        _go_to = std::make_unique<GoTo>();
        _token_store += _go_to->on_selected += [&](uint32_t index)
        {
            if (_go_to->name() == "Item")
            {
                on_select_item(index);
            }
            else
            {
                on_select_room(index);
            }
        };

        _toolbar = std::make_unique<Toolbar>();
        _toolbar->add_tool("Measure", "|....|");
        _token_store += _toolbar->on_tool_clicked += [this](const std::string& tool)
        {
            if (tool == "Measure")
            {
                on_tool_selected(Tool::Measure);
            }
        };

        _tooltip = std::make_unique<Tooltip>();
        _map_tooltip = std::make_unique<Tooltip>();

        _context_menu->on_add_waypoint += on_add_waypoint;
        _context_menu->on_add_mid_waypoint += on_add_mid_waypoint;
        _context_menu->on_remove_waypoint += on_remove_waypoint;
        _context_menu->on_orbit_here += on_orbit;
        _context_menu->on_hide += on_hide;
        _context_menu->set_remove_enabled(false);
        _context_menu->set_hide_enabled(false);

        _level_info = std::make_unique<LevelInfo>(*texture_storage);
        _token_store += _level_info->on_toggle_settings += [&]() { _settings_window->toggle_visibility(); };

        _token_store += _settings_window->on_vsync += [&](bool value)
        {
            _settings.vsync = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_go_to_lara += [&](bool value)
        {
            _settings.go_to_lara = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_invert_map_controls += [&](bool value)
        {
            _settings.invert_map_controls = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_items_startup += [&](bool value)
        {
            _settings.items_startup = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_triggers_startup += [&](bool value)
        {
            _settings.triggers_startup = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_rooms_startup += [&](bool value)
        {
            _settings.rooms_startup = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_auto_orbit += [&](bool value)
        {
            _settings.auto_orbit = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_invert_vertical_pan += [&](bool value)
        {
            _settings.invert_vertical_pan = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_sensitivity_changed += [&](float value)
        {
            _settings.camera_sensitivity = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_movement_speed_changed += [&](float value)
        {
            _settings.camera_movement_speed = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_camera_acceleration += [&](bool value)
        {
            _settings.camera_acceleration = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_camera_acceleration_rate += [&](float value)
        {
            _settings.camera_acceleration_rate = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_camera_display_degrees += [&](bool value)
        {
            _settings.camera_display_degrees = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_randomizer_tools += [&](bool value)
        {
            _settings.randomizer_tools = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_max_recent_files += [&](uint32_t value)
        {
            _settings.max_recent_files = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_background_colour += [&](Colour value)
        {
            _settings.background_colour = value;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_minimap_colours += [&](MapColours colours)
        {
            _settings.map_colours = colours;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_default_route_colour += [&](const Colour& colour)
        {
            _settings.route_colour = colour;
            on_settings(_settings);
        };
        _token_store += _settings_window->on_default_waypoint_colour += [&](const Colour& colour)
        {
            _settings.waypoint_colour = colour;
            on_settings(_settings);
        };

        _camera_position = std::make_unique<CameraPosition>();
        _camera_position->on_position_changed += on_camera_position;
        _camera_position->on_rotation_changed += on_camera_rotation;

        _console = std::make_unique<Console>();
        _console->on_command += on_command;

        _map_renderer = map_renderer_source(window.size());
        _token_store += _map_renderer->on_sector_hover += [this](const std::shared_ptr<ISector>& sector)
        {
            on_sector_hover(sector);

            if (!sector)
            {
                _map_tooltip->set_visible(false);
                return;
            }

            std::string text = std::format("X: {}, Z: {}\n", sector->x(), sector->z());
            if (has_flag(sector->flags(), SectorFlag::RoomAbove))
            {
                text += std::format("Above: {}", sector->room_above());
            }
            if (has_flag(sector->flags(), SectorFlag::RoomBelow))
            {
                text += std::format("{}Below: {}", has_flag(sector->flags(), SectorFlag::RoomAbove) ? ", " : "", sector->room_below());
            }
            _map_tooltip->set_text(text);
            _map_tooltip->set_visible(!text.empty());
        };
    }

    void ViewerUI::clear_minimap_highlight()
    {
        _map_renderer->clear_highlight();
    }

    std::shared_ptr<ISector> ViewerUI::current_minimap_sector() const
    {
        return _map_renderer->sector_at_cursor();
    }

    bool ViewerUI::is_input_active() const
    {
        return ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantTextInput;
    }

    bool ViewerUI::is_cursor_over() const
    {
        return (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouseUnlessPopupClose) || (_map_renderer->loaded() && _map_renderer->cursor_is_over_control());
    }

    void ViewerUI::generate_tool_window()
    {
        _view_options->on_toggle_changed += on_toggle_changed;
        _view_options->on_scalar_changed += on_scalar_changed;
        _view_options->on_alternate_group += on_alternate_group;

        _room_navigator = std::make_unique<RoomNavigator>();
        _room_navigator->on_room_selected += on_select_room;

        _camera_controls->on_reset += on_camera_reset;
        _camera_controls->on_mode_selected += on_camera_mode;
        _camera_controls->on_projection_mode_selected += on_camera_projection_mode;
    }

    void ViewerUI::render()
    {
        if (!_visible)
        {
            return;
        }

        _tooltip->render();
        _map_tooltip->render();
        _map_renderer->render();
        _view_options->render();
        _room_navigator->render();
        _camera_controls->render();
        _camera_position->render();
        _settings_window->render();
        _context_menu->render();
        _go_to->render();
        _level_info->render();
        _console->render();
        _toolbar->render();

        if (_show_measure)
        {
            const auto vp = ImGui::GetMainViewport();
            if (!(_measure_position.x < 0 || _measure_position.y < 0 || _measure_position.x > vp->Size.x || _measure_position.y > vp->Size.y))
            {
                ImGui::SetNextWindowPos(vp->Pos + ImVec2(_measure_position.x, _measure_position.y));
                if (ImGui::Begin("##measure", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text(_measure_text.c_str());
                }
                ImGui::End();
            }
        }
    }

    void ViewerUI::set_alternate_group(uint32_t value, bool enabled)
    {
        _view_options->set_alternate_group(value, enabled);
    }

    void ViewerUI::set_alternate_groups(const std::set<uint32_t>& groups)
    {
        _view_options->set_alternate_groups(groups);
    }

    void ViewerUI::set_camera_position(const DirectX::SimpleMath::Vector3& position)
    {
        _camera_position->set_position(position);
    }

    void ViewerUI::set_camera_rotation(float yaw, float pitch)
    {
        _camera_position->set_rotation(yaw, pitch);
    }

    void ViewerUI::set_camera_mode(CameraMode mode)
    {
        _camera_controls->set_mode(mode);
    }

    void ViewerUI::set_camera_projection_mode(ProjectionMode mode)
    {
        _camera_controls->set_projection_mode(mode);
    }

    void ViewerUI::set_flip_enabled(bool value)
    {
        _view_options->set_flip_enabled(value);
    }

    void ViewerUI::set_hide_enabled(bool value)
    {
        _context_menu->set_hide_enabled(value);
    }

    void ViewerUI::set_host_size(const Size& size)
    {
        _map_renderer->set_window_size(size);
    }

    void ViewerUI::set_level(const std::string& name, trlevel::LevelVersion version)
    {
        _level_info->set_level(name);
        _level_info->set_level_version(version);
    }

    void ViewerUI::set_max_rooms(uint32_t rooms)
    {
        _room_navigator->set_max_rooms(rooms);
    }

    void ViewerUI::set_measure_distance(float distance)
    {
        _measure_text = std::format("{:.2f}", distance);
    }

    void ViewerUI::set_measure_position(const Point& position)
    {
        _measure_position = position;
    }

    void ViewerUI::set_minimap_highlight(uint16_t x, uint16_t z)
    {
        _map_renderer->set_highlight(x, z);
    }

    void ViewerUI::set_pick(const PickInfo& info, const PickResult& result)
    {
        // TODO: Context menu visible?
        _tooltip->set_visible(result.hit && _show_tooltip && !_context_menu->visible());
        if (result.hit)
        {
            _map_tooltip->set_visible(false);
            _tooltip->set_text(result.text);
            _tooltip->set_text_colour(pick_to_colour(result));
        }
    }

    void ViewerUI::set_remove_waypoint_enabled(bool value)
    {
        _context_menu->set_remove_enabled(value);
    }

    void ViewerUI::set_settings(const UserSettings& settings)
    {
        _settings = settings;
        _settings_window->set_auto_orbit(settings.auto_orbit);
        _settings_window->set_go_to_lara(settings.go_to_lara);
        _settings_window->set_invert_map_controls(settings.invert_map_controls);
        _settings_window->set_items_startup(settings.items_startup);
        _settings_window->set_triggers_startup(settings.triggers_startup);
        _settings_window->set_rooms_startup(settings.rooms_startup);
        _settings_window->set_vsync(settings.vsync);
        _settings_window->set_invert_vertical_pan(settings.invert_vertical_pan);
        _settings_window->set_movement_speed(settings.camera_movement_speed);
        _settings_window->set_sensitivity(settings.camera_sensitivity);
        _settings_window->set_camera_acceleration(settings.camera_acceleration);
        _settings_window->set_camera_acceleration_rate(settings.camera_acceleration_rate);
        _settings_window->set_camera_display_degrees(settings.camera_display_degrees);
        _settings_window->set_randomizer_tools(settings.randomizer_tools);
        _settings_window->set_max_recent_files(settings.max_recent_files);
        _settings_window->set_background_colour(settings.background_colour);
        _settings_window->set_map_colours(settings.map_colours);
        _settings_window->set_default_route_colour(settings.route_colour);
        _settings_window->set_default_waypoint_colour(settings.waypoint_colour);
        _camera_position->set_display_degrees(settings.camera_display_degrees);
        _map_renderer->set_colours(settings.map_colours);
    }

    void ViewerUI::set_selected_item(uint32_t index)
    {
        _selected_item = index;
    }

    void ViewerUI::set_selected_room(const std::shared_ptr<IRoom>& room)
    {
        _room_navigator->set_selected_room(room->number());
        _selected_room = room->number();
        _map_renderer->load(room);
    }

    void ViewerUI::set_show_context_menu(bool value)
    {
        _context_menu->set_visible(value);
        if (value)
        {
            _tooltip->set_visible(false);
        }
    }

    void ViewerUI::set_show_measure(bool value)
    {
        _show_measure = value;
    }

    void ViewerUI::set_show_minimap(bool value)
    {
        _map_renderer->set_visible(value);
    }

    void ViewerUI::set_show_tooltip(bool value)
    {
        _show_tooltip = value;
        _tooltip->set_visible(_tooltip->visible() && _show_tooltip);
        _map_tooltip->set_visible(_map_tooltip->visible() && _show_tooltip);
    }

    void ViewerUI::set_use_alternate_groups(bool value)
    {
        _view_options->set_use_alternate_groups(value);
    }

    void ViewerUI::set_visible(bool value)
    {
        _visible = value;
    }

    bool ViewerUI::show_context_menu() const
    {
        return _context_menu->visible();
    }

    void ViewerUI::toggle_settings_visibility()
    {
        _settings_window->toggle_visibility();
    }

    void ViewerUI::print_console(const std::string& text)
    {
        _console->print(text);
    }

    void ViewerUI::set_mid_waypoint_enabled(bool value)
    {
        _context_menu->set_mid_waypoint_enabled(value);
    }

    void ViewerUI::set_scalar(const std::string& name, int32_t value)
    {
        _view_options->set_scalar(name, value);
    }

    void ViewerUI::set_toggle(const std::string& name, bool value)
    {
        _view_options->set_toggle(name, value);
    }

    bool ViewerUI::toggle(const std::string& name) const
    {
        return _view_options->toggle(name);
    }
}
