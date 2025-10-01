#include "trview_imgui.h"

namespace trview
{
    void imgui_header_row(std::vector<ImGuiHeader> headers)
    {
        for (const auto& header : headers)
        {
            ImGui::TableSetupColumn(header.name.c_str(), ImGuiTableColumnFlags_WidthStretch, header.width);
        }
        ImGui::TableSetupScrollFreeze(0, 1);
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

    void ImGuiAnchor::check_resize(ImVec2 intended_client_size)
    {
        if (last_padding.has_value() && last_position.has_value() && last_size.has_value() && !last_client_size.has_value())
        {
            const auto new_size = *last_padding + intended_client_size;
            ImGui::SetNextWindowSize(new_size, ImGuiCond_Always);
            if (!docked)
            {
                ImGui::SetNextWindowPos(*last_position + ImVec2(last_size->x, 0), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
            }
            last_client_size = intended_client_size;
        }

        // Regular resizing:
        if (last_padding.has_value() && last_client_size.has_value() && (*last_client_size != intended_client_size))
        {
            const auto new_size = *last_padding + intended_client_size;
            ImGui::SetNextWindowSize(new_size, ImGuiCond_Always);
            if (!docked)
            {
                ImGui::SetNextWindowPos(*last_position + ImVec2(last_padding->x + last_client_size->x, 0), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
            }
        }
    }

    void ImGuiAnchor::record_position(ImVec2 intended_client_size)
    {
        if (last_client_size.has_value())
        {
            last_client_size = intended_client_size;
        }
        last_position = ImGui::GetWindowPos();
        const auto cursorPos = ImGui::GetCursorPos();
        last_padding = ImVec2(cursorPos.x * 2, cursorPos.y + cursorPos.x);
        docked = ImGui::IsWindowDocked();
    }

    void ImGuiAnchor::record_size()
    {
        last_size = ImGui::GetWindowSize();
    }
}
