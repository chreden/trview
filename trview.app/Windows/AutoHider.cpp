#include "AutoHider.h"

namespace trview
{
    bool AutoHider::changed() const
    {
        return _enabled_changed;
    }

    void AutoHider::check_focus()
    {
        const bool focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
        _enabled_changed = _enabled && (!_was_focused && focused);
        _was_focused = focused;
    }

    void AutoHider::render()
    {
        ImGui::SameLine();
        if (ImGui::Checkbox("Auto Hide", &_enabled))
        {
            _enabled_changed = true;
        }
    }
}
