#include "ContextMenu.h"
#include "../Windows/IItemsWindowManager.h"

namespace trview
{
    IContextMenu::~IContextMenu()
    {
    }

    ContextMenu::ContextMenu(const std::weak_ptr<IItemsWindowManager>& items_window_manager)
        : _items_window_manager(items_window_manager)
    {
    }

    void ContextMenu::render()
    {
        if (_can_show && ImGui::BeginPopupContextVoid())
        {
            _visible = true;
            if (ImGui::MenuItem(Names::add_waypoint.c_str()))
            {
                on_add_waypoint();
            }
            if (ImGui::MenuItem(Names::add_mid_waypoint.c_str(), nullptr, nullptr, _mid_enabled))
            {
                on_add_mid_waypoint();
            }
            if (ImGui::MenuItem(Names::remove_waypoint.c_str(), nullptr, nullptr, _remove_enabled))
            {
                on_remove_waypoint();
            }
            if (ImGui::MenuItem(Names::orbit.c_str()))
            {
                on_orbit_here();
            }
            if (ImGui::MenuItem(Names::hide.c_str(), nullptr, nullptr, _hide_enabled))
            {
                on_hide();
            }

            if (ImGui::BeginMenu(Names::copy.c_str()))
            {
                if (ImGui::MenuItem(Names::copy_position.c_str()))
                {
                    on_copy(CopyType::Position);
                }
                if (ImGui::MenuItem(Names::copy_number.c_str()))
                {
                    on_copy(CopyType::Number);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(Names::trigger_references.c_str(), !_triggered_by.empty()))
            {
                for (const auto& trigger : _triggered_by)
                {
                    if (const auto trigger_ptr = trigger.lock())
                    {
                        if (ImGui::MenuItem(std::format("{} {}", trigger_type_name(trigger_ptr->type()), trigger_ptr->number()).c_str()))
                        {
                            on_trigger_selected(trigger);
                        }
                    }
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(Names::filter.c_str(), _tile_filter_enabled))
            {
                if (ImGui::BeginMenu("By Tile in..."))
                {
                    if (ImGui::BeginMenu("Items..."))
                    {
                        if (auto items_windows = _items_window_manager.lock())
                        {
                            if (ImGui::MenuItem("New Window"))
                            {
                                on_filter_items_to_tile(items_windows->create_window());
                            }

                            for (const auto& window : items_windows->windows())
                            {
                                if (auto actual_window = window.lock())
                                {
                                    if (ImGui::MenuItem(actual_window->name().c_str()))
                                    {
                                        on_filter_items_to_tile(window);
                                    }
                                }
                            }
                        }
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }
        else
        {
            _visible = false;
        }
    }

    void ContextMenu::set_visible(bool value)
    {
        _can_show = value;
    }

    void ContextMenu::set_remove_enabled(bool value)
    {
        _remove_enabled = value;
    }

    void ContextMenu::set_hide_enabled(bool value)
    {
        _hide_enabled = value;
    }

    void ContextMenu::set_mid_waypoint_enabled(bool value)
    {
        _mid_enabled = value;
    }

    bool ContextMenu::visible() const
    {
        return _visible;
    }

    void ContextMenu::set_triggered_by(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _triggered_by = triggers;
    }

    void ContextMenu::set_tile_filter_enabled(bool value)
    {
        _tile_filter_enabled = value;
    }
}
