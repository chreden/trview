#include "trview_imgui.h"

namespace trview
{
    void imgui_scroll_to_item(bool selected, bool& should_scroll_to)
    {
        if (selected && should_scroll_to)
        {
            const auto pos = ImGui::GetCurrentWindow()->DC.CursorPos;
            if (!ImGui::IsRectVisible(pos, pos + ImVec2(1, 1)))
            {
                ImGui::SetScrollHereY();
            }
            should_scroll_to = false;
        }
    }
}
