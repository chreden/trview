#include "trview_imgui.h"

namespace trview
{
    ImGuiScroller::ImGuiScroller()
    {
        const auto window = ImGui::GetCurrentWindow();
        _cursor_pos = window->DC.CursorPos;
    }

    void ImGuiScroller::scroll_to_item()
    {
        if (!ImGui::IsRectVisible(_cursor_pos, _cursor_pos + ImVec2(1, 1)))
        {
            ImGui::SetScrollHereY();
        }
    }

    void ImGuiScroller::fix_scroll()
    {
        const auto window = ImGui::GetCurrentWindow();
        if (!ImGui::IsRectVisible(_cursor_pos, _cursor_pos + ImVec2(1, 1)))
        {
            ImGui::SetScrollY(window->Scroll.y - ImGui::GetTextLineHeightWithSpacing());
        }
    }
}
