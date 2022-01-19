#include "ControlTracker.h"
#include <external/imgui/imgui.h>

namespace trview
{
    void ControlTracker::track(const std::string& name)
    {
        auto wp = ImGui::GetWindowPos();
        auto min = ImGui::GetItemRectMin();
        auto max = ImGui::GetItemRectMax();
        _tracked_controls[name] = { wp.x + min.x, wp.y + min.y, wp.x + (max.x - min.x), wp.y + (max.y - min.y) };
    }

    std::optional<ControlTracker::Entry> ControlTracker::find_tracked_control(const std::string& name) const
    {
        auto found = _tracked_controls.find(name);
        if (found == _tracked_controls.end())
        {
            return {};
        }
        return found->second;
    }
}