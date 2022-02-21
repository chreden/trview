#include "ContextMenu.h"

namespace trview
{
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
}
