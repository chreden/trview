#include "ViewerUI.h"
#include <trview.app/Graphics/ILevelTextureStorage.h>
#include "GoTo.h"
#include <trview.app/UI/ContextMenu.h>
#include <trview.input/WindowTester.h>
#include <trview.common/Windows/Shortcuts.h>
#include "../Routing/IRoute.h"
#include "../Windows/IViewer.h"
#include <ranges>
#include <trview.common/Messages/Message.h>

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        template <typename PointType>
        bool is_on_screen(const PointType& point, const ImGuiViewport& viewport)
        {
            return !(point.x < 0 || point.y < 0 || point.x > viewport.Size.x || point.y > viewport.Size.y);
        }
    }

    IViewerUI::~IViewerUI()
    {
    }

// Disabled as const auto all = { items, triggers, rooms } was being marked as unreachable in debug only.
#pragma warning(push)
#pragma warning(disable: 4702)
    ViewerUI::ViewerUI(const Window& window, const std::shared_ptr<ITextureStorage>& texture_storage,
        const std::shared_ptr<IShortcuts>& shortcuts,
        const IMapRenderer::Source& map_renderer_source,
        const std::shared_ptr<ISettingsWindow>& settings_window,
        std::unique_ptr<IViewOptions> view_options,
        std::unique_ptr<IContextMenu> context_menu,
        std::unique_ptr<ICameraControls> camera_controls,
        std::unique_ptr<IToolbar> toolbar,
        const std::weak_ptr<IMessageSystem>& messaging)
        : _mouse(window, std::make_unique<input::WindowTester>(window)), _window(window), _camera_controls(std::move(camera_controls)),
        _view_options(std::move(view_options)), _settings_window(settings_window), _context_menu(std::move(context_menu)), _toolbar(std::move(toolbar)),
        _messaging(messaging)
    {
        _token_store += shortcuts->add_shortcut(true, 'F') += [&]()
        {
            if (!is_input_active())
            {
                _tooltip->set_visible(false);
                _go_to->toggle_visible();
                if (auto level = _level.lock())
                {
                    const auto items = level->items()
                        | std::views::transform([](auto&& i) { return i.lock(); })
                        | std::views::filter([](auto&& i) { return i != nullptr; })
                        | std::views::transform([](auto&& i) -> GoTo::GoToItem { return { .number = i->number(), .name = i->type(), .item = i }; })
                        | std::ranges::to<std::vector>();

                    const auto triggers = level->triggers()
                        | std::views::transform([](auto&& t) { return t.lock(); })
                        | std::views::filter([](auto&& t) { return t != nullptr; })
                        | std::views::transform([](auto&& t) -> GoTo::GoToItem { return { .number = t->number(), .name = to_string(t->type()), .item = t }; })
                        | std::ranges::to<std::vector>();

                    const auto rooms = level->rooms()
                        | std::views::transform([](auto&& r) { return r.lock(); })
                        | std::views::filter([](auto&& r) { return r != nullptr; })
                        | std::views::transform([](auto&& r) -> GoTo::GoToItem { return { .number = r->number(), .name = std::format("Room {}", r->number()), .item = r }; })
                        | std::ranges::to<std::vector>();

                    const auto all = { items, triggers, rooms };
                    _go_to->set_items(all | std::views::join | std::ranges::to<std::vector>());
                }
            }
        };

        generate_tool_window();

        _go_to = std::make_unique<GoTo>();
        _token_store += _go_to->on_selected += [&](const GoTo::GoToItem& item)
        {
            _tooltip->set_visible(false);
            if (std::holds_alternative<std::weak_ptr<IItem>>(item.item))
            {
                on_select_item(std::get<std::weak_ptr<IItem>>(item.item));
            }
            else if (std::holds_alternative<std::weak_ptr<ITrigger>>(item.item))
            {
                on_select_trigger(std::get<std::weak_ptr<ITrigger>>(item.item));
            }
            else if (std::holds_alternative<std::weak_ptr<IRoom>>(item.item))
            {
                on_select_room(std::get<std::weak_ptr<IRoom>>(item.item));
            }
        };

        _toolbar->add_tool("Measure");
        _token_store += _toolbar->on_tool_clicked += [this](const std::string& tool)
        {
            if (tool == "Measure")
            {
                on_tool_selected(Tool::Measure);
            }
        };

        _tooltip = std::make_unique<Tooltip>();
        _context_menu->on_add_waypoint += on_add_waypoint;
        _context_menu->on_add_mid_waypoint += on_add_mid_waypoint;
        _context_menu->on_remove_waypoint += on_remove_waypoint;
        _context_menu->on_orbit_here += on_orbit;
        _context_menu->on_hide += on_hide;
        _context_menu->on_copy += on_copy;
        _context_menu->on_trigger_selected += on_select_trigger;
        _context_menu->on_filter_items_to_tile += on_filter_items_to_tile;
        _context_menu->set_remove_enabled(false);
        _context_menu->set_hide_enabled(false);

        _level_info = std::make_unique<LevelInfo>(*texture_storage);
        _token_store += _level_info->on_toggle_settings += [&]() { _settings_window->toggle_visibility(); };

        _settings_window->on_font += on_font;
        _settings_window->on_linear_filtering += on_linear_filtering;

        _camera_position = std::make_unique<CameraPosition>();
        _camera_position->on_position_changed += on_camera_position;
        _camera_position->on_rotation_changed += on_camera_rotation;
        _token_store += _camera_position->on_hidden += [this]()
            {
                _settings.camera_position_window = false;
                if (auto ms = _messaging.lock())
                {
                    ms->send_message(Message{ .type = "settings", .data = std::make_shared<MessageData<UserSettings>>(_settings) });
                }
            };

        _map_renderer = map_renderer_source();
        _map_renderer->on_sector_hover += on_sector_hover;
        _map_renderer->on_room_selected += on_select_room;
        _map_renderer->on_trigger_selected += on_select_trigger;
    }
#pragma warning(pop)

    void ViewerUI::clear_minimap_highlight()
    {
        _map_renderer->clear_highlight();
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
        _token_store += _room_navigator->on_room_selected += [&](const auto index)
            {
                if (auto level = _level.lock())
                {
                    on_select_room(level->room(index));
                }
            };

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

        _map_renderer->render(true);
        _view_options->render();
        _room_navigator->render();
        _camera_controls->render();
        _camera_position->render();
        _settings_window->render();
        _context_menu->render();
        _go_to->render();
        _level_info->render();
        _toolbar->render();
        _tooltip->render();

        if (_show_measure)
        {
            const auto vp = ImGui::GetMainViewport();
            if (is_on_screen(_measure_position, *vp))
            {
                ImGui::SetNextWindowPos(vp->Pos + ImVec2(_measure_position.x, _measure_position.y));
                if (ImGui::Begin("##measure", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing))
                {
                    ImGui::Text(_measure_text.c_str());
                }
                ImGui::End();
            }
        }

        render_route_notes();
        render_scriptable_notes();
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

    void ViewerUI::set_camera_mode(ICamera::Mode mode)
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

    void ViewerUI::set_host_size_changed()
    {
        _map_renderer->reposition();
        _camera_position->reposition();
    }

    void ViewerUI::set_level(const std::weak_ptr<ILevel>& level)
    {
        _level = level;
        _map_renderer->load({});
        if (auto new_level = _level.lock())
        {
            _level_info->set_level(new_level->name());
            _level_info->set_level_version(new_level->version());
            _view_options->set_ng_plus_enabled(
                std::ranges::any_of(new_level->items(), [](auto&& i)
                    {
                        const auto item = i.lock();
                        return item && item->ng_plus() != std::nullopt;
                    }));
        }
        else
        {
            _level_info->set_level("");
        }
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

    void ViewerUI::set_pick(const PickResult& result)
    {
        // TODO: Context menu visible?
        _tooltip->set_visible(result.hit && _show_tooltip && !_context_menu->visible());
        if (result.hit)
        {
            _tooltip->set_text(result.text);
            _tooltip->set_text_colour(pick_to_colour(result));
        }
    }

    void ViewerUI::set_remove_waypoint_enabled(bool value)
    {
        _context_menu->set_remove_enabled(value);
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
        _map_renderer->set_show_tooltip(_show_tooltip);
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

    void ViewerUI::set_triggered_by(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _context_menu->set_triggered_by(triggers);
    }

    void ViewerUI::set_route(const std::weak_ptr<IRoute>& route)
    {
        _route = route;
    }

    void ViewerUI::render_route_notes()
    {
        const auto route = _route.lock();
        if (!route || !toggle(IViewer::Options::notes))
        {
            return;
        }

        const auto vp = ImGui::GetMainViewport();
        for (auto i = 0u; i < route->waypoints(); ++i)
        {
            if (const auto waypoint = route->waypoint(i).lock())
            {
                const auto notes = waypoint->notes();
                int diff = 0;
                if (i > 0)
                {
                    if (const auto previous = route->waypoint(i - 1).lock())
                    {
                        diff = static_cast<int>((waypoint->position().y - previous->position().y) * trlevel::Scale);
                    }
                }

                if (notes.empty() && diff == 0)
                {
                    continue;
                }

                const auto pos = waypoint->screen_position();
                if (is_on_screen(pos, *vp))
                {
                    ImGui::SetNextWindowPos(vp->Pos + ImVec2(pos.x, pos.y));
                    if (ImGui::Begin(std::format("##waypoint{}", i).c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing))
                    {
                        if (diff != 0)
                        {
                            const std::string sign = diff <= 0 ? "+" : "-";
                            const int clicks = static_cast<int>(round(diff / trlevel::Click));
                            ImGui::Text(std::format("{}{} ({}{} clicks)", sign, abs(diff), sign, abs(clicks)).c_str());
                        }
                        if (!notes.empty())
                        {
                            ImGui::Text(notes.c_str());
                        }
                    }
                    ImGui::End();
                }
            }
        }
    }

    void ViewerUI::render_scriptable_notes()
    {
        const auto level = _level.lock();
        if (!level)
        {
            return;
        }

        const auto vp = ImGui::GetMainViewport();

        uint32_t i = 0;
        for (const auto scriptable : level->scriptables())
        {
            if (const auto scriptable_ptr = scriptable.lock())
            {
                const auto notes = scriptable_ptr->notes();
                if (notes.empty())
                {
                    continue;
                }

                const auto pos = scriptable_ptr->screen_position();
                if (is_on_screen(pos, *vp))
                {
                    ImGui::SetNextWindowPos(vp->Pos + ImVec2(pos.x, pos.y));
                    if (ImGui::Begin(std::format("##scriptable{}", i).c_str(), nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoFocusOnAppearing))
                    {
                        ImGui::Text(notes.c_str());
                    }
                    ImGui::End();
                }
            }
        }
    }

    void ViewerUI::set_show_camera_position(bool value)
    {
        _camera_position->set_visible(value);
    }

    void ViewerUI::reset_layout()
    {
        _map_renderer->reset();
        _camera_position->reset();
    }

    void ViewerUI::set_tile_filter_enabled(bool value)
    {
        _context_menu->set_tile_filter_enabled(value);
    }

    void ViewerUI::receive_message(const Message& message)
    {
        if (message.type == "settings")
        {
            _settings = std::static_pointer_cast<MessageData<UserSettings>>(message.data)->value;
            _camera_position->set_display_degrees(_settings.camera_display_degrees);
            _camera_position->set_visible(_settings.camera_position_window);
            for (const auto& toggle : _settings.toggles)
            {
                set_toggle(toggle.first, toggle.second);
            }
        }
    }
}
