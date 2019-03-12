#include "ViewerUI.h"
#include <trview.ui/Window.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.app/Graphics/ILevelTextureStorage.h>
#include "GoTo.h"
#include <trview.app/UI/ContextMenu.h>
#include <sstream>
#include <iomanip>

using namespace trview::ui;

namespace trview
{
    ViewerUI::ViewerUI(const Window& window, const graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::FontFactory& font_factory, const ITextureStorage& texture_storage)
        : _mouse(window), _window(window), _keyboard(window)
    {
        _control = std::make_unique<ui::Window>(Point(), window.size(), Colour::Transparent);
        _control->set_handles_input(false);
        _ui_input = std::make_unique<Input>(window, *_control);

        _token_store += _keyboard.on_key_down += [&](auto key)
        {
            if (_keyboard.control())
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

        auto picking = std::make_unique<ui::Label>(Point(500, 0), Size(38, 30), Colour(0.2f, 0.2f, 0.2f), L"0", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre);
        picking->set_visible(false);
        picking->set_handles_input(false);
        _tooltip = _control->add_child(std::move(picking));

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

        // Create the renderer for the UI based on the controls created.
        _ui_renderer = std::make_unique<ui::render::Renderer>(device, shader_storage, font_factory, window.size());
        _ui_renderer->load(_control.get());

        _map_renderer = std::make_unique<ui::render::MapRenderer>(device, shader_storage, window.size());
        _map_renderer->on_sector_hover += on_sector_hover;

        _camera_position = std::make_unique<CameraPosition>(*_control);
    }

    void ViewerUI::clear_minimap_highlight()
    {
        _map_renderer->clear_highlight();
    }

    std::shared_ptr<Sector> ViewerUI::current_minimap_sector() const
    {
        return _map_renderer->sector_at_cursor();
    }

    bool ViewerUI::go_to_room_visible() const
    {
        return _go_to->visible();
    }

    bool ViewerUI::is_cursor_over() const
    {
        return _control->is_mouse_over(client_cursor_position(_window))
            || (_map_renderer->loaded() && _map_renderer->cursor_is_over_control());
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
        _camera_controls->on_reset += on_camera_reset;
        _camera_controls->on_mode_selected += on_camera_mode;
        _camera_controls->on_sensitivity_changed += on_camera_sensitivity;
        _camera_controls->on_movement_speed_changed += on_camera_movement_speed;
    }

    void ViewerUI::render(const graphics::Device& device)
    {
        _ui_renderer->render(device.context());
        
        Point point = client_cursor_position(_window);
        _map_renderer->set_cursor_position(point);
        _map_renderer->render(device.context());
    }

    void ViewerUI::set_alternate_group(uint16_t value, bool enabled)
    {
        _flipmaps->set_alternate_group(value, enabled);
    }

    void ViewerUI::set_alternate_groups(const std::set<uint16_t>& groups)
    {
        _flipmaps->set_alternate_groups(groups);
    }

    void ViewerUI::set_camera_movement_speed(float value)
    {
        _camera_controls->set_movement_speed(value);
    }

    void ViewerUI::set_camera_position(const DirectX::SimpleMath::Vector3& position)
    {
        _camera_position->set_position(position);
    }

    void ViewerUI::set_camera_sensitivity(float value)
    {
        _camera_controls->set_sensitivity(value);
    }

    void ViewerUI::set_camera_mode(CameraMode mode)
    {
        _camera_controls->set_mode(mode);
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

    void ViewerUI::set_host_size(const Size& size)
    {
        _control->set_size(size);
        _ui_renderer->set_host_size(size);
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
            auto screen_pos = info.screen_position;
            _tooltip->set_position(Point(screen_pos.x - _tooltip->size().width, screen_pos.y - _tooltip->size().height));
            _tooltip->set_text(pick_to_string(result));
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
        _room_navigator->set_show_hidden_geometry(value);
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

    void ViewerUI::toggle_settings_visibility()
    {
        _settings_window->toggle_visibility();
    }
}
