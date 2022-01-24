#include "ContextMenu.h"
#include <external/imgui/imgui.h>
#include <external/imgui/imgui_internal.h>

using namespace trview::ui;

namespace trview
{
    const std::string ContextMenu::Names::hide_button{ "Hide" };
    const std::string ContextMenu::Names::orbit_button{ "Orbit" };
    const std::string ContextMenu::Names::add_waypoint_button{ "AddWaypoint" };
    const std::string ContextMenu::Names::add_mid_waypoint_button{ "AddMidWaypoint" };
    const std::string ContextMenu::Names::remove_waypoint_button{ "RemoveWaypoint" };

    void ContextMenu::render()
    {
        if (_can_show && ImGui::BeginPopupContextVoid())
        {
            _visible = true;
            if (ImGui::MenuItem("Add Waypoint"))
            {
                on_add_waypoint();
            }
            if (ImGui::MenuItem("Add Mid Waypoint", nullptr, nullptr, _mid_enabled))
            {
                on_add_mid_waypoint();
            }
            if (ImGui::MenuItem("Remove Waypoint", nullptr, nullptr, _remove_enabled))
            {
                on_remove_waypoint();
            }
            if (ImGui::MenuItem("Orbit Here"))
            {
                on_orbit_here();
            }
            if (ImGui::MenuItem("Hide", nullptr, nullptr, _hide_enabled))
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
