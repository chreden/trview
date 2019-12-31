#include "ViewerUI.h"
#include <trview.ui/Window.h>
#include <trview.ui/Label.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.app/Graphics/ILevelTextureStorage.h>
#include "GoTo.h"
#include <trview.app/UI/ContextMenu.h>
#include <sstream>
#include <iomanip>
#include <trview.input/WindowTester.h>

using namespace trview::ui;

namespace trview
{
    ViewerUI::ViewerUI(const Window& window, const graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::FontFactory& font_factory, const ITextureStorage& texture_storage)
        : _mouse(window, std::make_unique<input::WindowTester>(window)), _window(window), _keyboard(window)
    {
        _control = std::make_unique<ui::Window>(window.size(), Colour::Transparent);

        register_change_detection(_control.get());

        _control->set_handles_input(false);
        _ui_input = std::make_unique<Input>(window, *_control);

        _token_store += _mouse.mouse_move += [&](long, long)
        {
            _map_renderer->set_cursor_position(client_cursor_position(_window));
            if (_map_tooltip && _map_tooltip->visible())
            {
                _map_tooltip->set_position(client_cursor_position(_window));
            }
        };

        _token_store += _keyboard.on_key_down += [&](auto key, bool control)
        {
            if (control)
            {
                std::wstring name;
                switch (key)
                {
                case 'G':
                    name = L"Room";
                    break;
                case 'M':
                    name = L"Item";
                    break;
                default:
                    return;
                }

                if (!_go_to->visible())
                {
                    _go_to->set_name(name);
                    _go_to->toggle_visible();
                }
                else if (_go_to->name() == name)
                {
                    _go_to->toggle_visible();
                }
                else
                {
                    _go_to->set_name(name);
                }
            }
        };

        generate_tool_window(texture_storage);

        _go_to = std::make_unique<GoTo>(*_control.get());
        _token_store += _go_to->on_selected += [&](uint32_t index)
        {
            if (_go_to->name() == L"Item")
            {
                on_select_item(index);
            }
            else
            {
                on_select_room(index);
            }
        };

        _toolbar = std::make_unique<Toolbar>(*_control);
        _toolbar->add_tool(L"Measure", L"|....|");
        _token_store += _toolbar->on_tool_clicked += [this](const std::wstring& tool)
        {
            if (tool == L"Measure")
            {
                on_tool_selected(Tool::Measure);
            }
        };

        _tooltip = std::make_unique<Tooltip>(*_control);
        _map_tooltip = std::make_unique<Tooltip>(*_control);

        auto measure = std::make_unique<ui::Label>(Point(300, 100), Size(50, 30), Colour(1.0f, 0.2f, 0.2f, 0.2f), L"0", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre);
        measure->set_visible(false);
        _measure = _control->add_child(std::move(measure));

        _context_menu = std::make_unique<ContextMenu>(*_control);
        _context_menu->on_add_waypoint += on_add_waypoint;
        _context_menu->on_remove_waypoint += on_remove_waypoint;
        _context_menu->on_orbit_here += on_orbit;
        _context_menu->set_remove_enabled(false);

        _level_info = std::make_unique<LevelInfo>(*_control.get(), texture_storage);
        _token_store += _level_info->on_toggle_settings += [&]() { _settings_window->toggle_visibility(); };

        _settings_window = std::make_unique<SettingsWindow>(*_control.get());
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
        _settings_window->on_sensitivity_changed += on_camera_sensitivity;
        _settings_window->on_movement_speed_changed += on_camera_movement_speed;

        _camera_position = std::make_unique<CameraPosition>(*_control);
        _camera_position->on_position_changed += on_camera_position;

        // Create the renderer for the UI based on the controls created.
        _ui_renderer = std::make_unique<ui::render::Renderer>(device, shader_storage, font_factory, window.size());
        _ui_renderer->load(_control.get());

        _map_renderer = std::make_unique<ui::render::MapRenderer>(device, shader_storage, font_factory, window.size());
        _token_store += _map_renderer->on_sector_hover += [this](const std::shared_ptr<Sector>& sector)
        {
            on_ui_changed();
            on_sector_hover(sector);

            if (!sector)
            {
                _map_tooltip->set_visible(false);
                return;
            }

            std::wstring text;
            if (sector->flags & SectorFlag::RoomAbove)
            {
                text += L"Above: " + std::to_wstring(sector->room_above());
            }
            if (sector->flags & SectorFlag::RoomBelow)
            {
                text += ((sector->flags & SectorFlag::RoomAbove) ? L", " : L"") +
                    std::wstring(L"Below: ") + std::to_wstring(sector->room_below());
            }
            _map_tooltip->set_text(text);
            _map_tooltip->set_position(client_cursor_position(_window));
            _map_tooltip->set_visible(!text.empty());
        };
    }

    void ViewerUI::register_change_detection(Control* control)
    {
        control->on_invalidate += on_ui_changed;
        control->on_hierarchy_changed += on_ui_changed;
        _token_store += control->on_add_child += std::bind(&ViewerUI::register_change_detection, this, std::placeholders::_1);

        // Go through all of the control's children, as they may have been added previously.
        for (auto& child : control->child_elements())
        {
            register_change_detection(child);
        }
    }

    void ViewerUI::clear_minimap_highlight()
    {
        _map_renderer->clear_highlight();
    }

    std::shared_ptr<Sector> ViewerUI::current_minimap_sector() const
    {
        return _map_renderer->sector_at_cursor();
    }

    bool ViewerUI::is_input_active() const
    {
        const auto focus = _ui_input->focus_control();
        return focus && focus->visible() && focus->handles_input();
    }

    bool ViewerUI::is_cursor_over() const
    {
        return _control->is_mouse_over(client_cursor_position(_window))
            || (_map_renderer->loaded() && _map_renderer->cursor_is_over_control());
    }

    void ViewerUI::generate_tool_window(const ITextureStorage& texture_storage)
    {
        // This is the main tool window on the side of the screen.
        auto tool_window = std::make_unique<StackPanel>(Size(150.0f, 348.0f), Colour(0.5f, 0.0f, 0.0f, 0.0f), Size(5, 5));
        tool_window->set_margin(Size(5, 5));

        _view_options = std::make_unique<ViewOptions>(*tool_window, texture_storage);
        _view_options->on_highlight += on_highlight;
        _view_options->on_show_triggers += on_show_triggers;
        _view_options->on_show_hidden_geometry += on_show_hidden_geometry;
        _view_options->on_show_water += on_show_water;
        _view_options->on_depth_changed += on_depth_level_changed;
        _view_options->on_depth_enabled += on_depth;
        _view_options->on_flip += on_flip;
        _view_options->on_alternate_group += on_alternate_group;

        _room_navigator = std::make_unique<RoomNavigator>(*tool_window.get(), texture_storage);
        _room_navigator->on_room_selected += on_select_room;

        initialise_camera_controls(*tool_window);

        _control->add_child(std::move(tool_window));
    }

    void ViewerUI::initialise_camera_controls(ui::Control& parent)
    {
        _camera_controls = std::make_unique<CameraControls>(parent);
        _camera_controls->on_reset += on_camera_reset;
        _camera_controls->on_mode_selected += on_camera_mode;
        _camera_controls->on_projection_mode_selected += on_camera_projection_mode;
    }

    void ViewerUI::render(const graphics::Device& device)
    {
        _map_renderer->render(device.context());
        _ui_renderer->render(device.context());
    }

    void ViewerUI::set_alternate_group(uint32_t value, bool enabled)
    {
        _view_options->set_alternate_group(value, enabled);
    }

    void ViewerUI::set_alternate_groups(const std::set<uint32_t>& groups)
    {
        _view_options->set_alternate_groups(groups);
    }

    void ViewerUI::set_camera_movement_speed(float value)
    {
        _settings_window->set_movement_speed(value);
    }

    void ViewerUI::set_camera_position(const DirectX::SimpleMath::Vector3& position)
    {
        _camera_position->set_position(position);
    }

    void ViewerUI::set_camera_sensitivity(float value)
    {
        _settings_window->set_sensitivity(value);
    }

    void ViewerUI::set_camera_mode(CameraMode mode)
    {
        _camera_controls->set_mode(mode);
    }

    void ViewerUI::set_camera_projection_mode(ProjectionMode mode)
    {
        _camera_controls->set_projection_mode(mode);
    }

    void ViewerUI::set_depth_enabled(bool value)
    {
        _view_options->set_depth_enabled(value);
    }

    void ViewerUI::set_depth_level(int32_t value)
    {
        _view_options->set_depth(value);
    }

    void ViewerUI::set_flip(bool value)
    {
        _view_options->set_flip(value);
    }

    void ViewerUI::set_flip_enabled(bool value)
    {
        _view_options->set_flip_enabled(value);
    }

    void ViewerUI::set_highlight(bool value)
    {
        _view_options->set_highlight(value);
    }

    void ViewerUI::set_host_size(const Size& size)
    {
        _control->set_size(size);
        _ui_renderer->set_host_size(size);
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
        std::wstringstream stream;
        stream << std::fixed << std::setprecision(2) << distance;
        _measure->set_text(stream.str());
    }

    void ViewerUI::set_measure_position(const Point& position)
    {
        _measure->set_position(position);
    }

    void ViewerUI::set_minimap_highlight(uint16_t x, uint16_t z)
    {
        _map_renderer->set_highlight(x, z);
    }

    void ViewerUI::set_pick(const PickInfo& info, const PickResult& result)
    {
        using namespace DirectX;
        using namespace DirectX::SimpleMath;

        // Show the tooltip.
        _tooltip->set_visible(result.hit && _show_tooltip);
        if (result.hit)
        {
            _map_tooltip->set_visible(false);
            _tooltip->set_text(pick_to_string(result));
            _tooltip->set_position(info.screen_position);
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
        _settings_window->set_vsync(settings.vsync);
        _settings_window->set_invert_vertical_pan(settings.invert_vertical_pan);
    }

    void ViewerUI::set_selected_room(Room* room)
    {
        _room_navigator->set_selected_room(room->number());
        _room_navigator->set_room_info(room->info());
        _map_renderer->load(room);
    }

    void ViewerUI::set_show_context_menu(bool value)
    {
        _context_menu->set_visible(value);
        if (value)
        {
            _context_menu->set_position(client_cursor_position(_window));
        }
    }

    void ViewerUI::set_show_hidden_geometry(bool value)
    {
        _view_options->set_show_hidden_geometry(value);
    }

    void ViewerUI::set_show_measure(bool value)
    {
        _measure->set_visible(value);
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

    void ViewerUI::set_show_triggers(bool value)
    {
        _view_options->set_show_triggers(value);
    }

    void ViewerUI::set_show_water(bool value)
    {
        _view_options->set_show_water(value);
    }

    void ViewerUI::set_use_alternate_groups(bool value)
    {
        _view_options->set_use_alternate_groups(value);
    }

    void ViewerUI::set_visible(bool value)
    {
        _control->set_visible(value);
    }

    bool ViewerUI::show_hidden_geometry() const
    {
        return _view_options->show_hidden_geometry();
    }

    bool ViewerUI::show_triggers() const
    {
        return _view_options->show_triggers();
    }

    bool ViewerUI::show_water() const
    {
        return _view_options->show_water();
    }

    void ViewerUI::toggle_settings_visibility()
    {
        _settings_window->toggle_visibility();
    }
}
