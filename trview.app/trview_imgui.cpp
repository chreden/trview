#include "trview_imgui.h"

namespace trview
{
    void imgui_header_row(std::vector<ImGuiHeader> headers)
    {
        for (const auto& header : headers)
        {
            ImGui::TableSetupColumn(header.name.c_str(), ImGuiTableColumnFlags_WidthFixed, header.width);
        }
        ImGui::TableSetupScrollFreeze(1, 1);
        ImGui::TableNextRow(ImGuiTableRowFlags_Headers, ImGui::TableGetHeaderRowHeight());

        int column_n = 0;
        for (const auto& header : headers)
        {
            if (!ImGui::TableSetColumnIndex(column_n++))
            {
                continue;
            }

            if (header.set_checked)
            {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

                bool value = header.checked;
                if (ImGui::Checkbox("##checkall", &value))
                {
                    header.set_checked(value);
                }

                ImGui::PopStyleVar();
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            }
            ImGui::TableHeader(header.name.c_str());
        }
    }

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
