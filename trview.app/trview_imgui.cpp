#include "trview_imgui.h"
#include <trview.common/Windows/IClipboard.h>

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

    bool ImGuiAnchor::check_resize(ImVec2 intended_client_size)
    {
        bool resized = false;
        if (last_padding.has_value() && last_position.has_value() && last_size.has_value() && !last_client_size.has_value())
        {
            const auto new_size = *last_padding + intended_client_size;
            ImGui::SetNextWindowSize(new_size, ImGuiCond_Always);
            resized = true;
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
            resized = true;
            if (!docked)
            {
                ImGui::SetNextWindowPos(*last_position + ImVec2(last_padding->x + last_client_size->x, 0), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
            }
        }

        if (reposition && in_window_offset.has_value())
        {
            const auto window = ImGui::GetCurrentWindow();
            const auto viewport = ImGui::GetMainViewport();
            if (!docked && window->Viewport == viewport)
            {
                switch (anchor)
                {
                case Anchor::TopRight:
                    ImGui::SetNextWindowPos(window->Viewport->Pos + ImVec2(window->Viewport->Size.x, 0) + in_window_offset.value(), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
                    break;
                case Anchor::BottomLeft:
                    ImGui::SetNextWindowPos(window->Viewport->Pos + ImVec2(0, window->Viewport->Size.y) + in_window_offset.value(), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
                    break;
                case Anchor::BottomRight:
                    ImGui::SetNextWindowPos(window->Viewport->Pos + window->Viewport->Size + in_window_offset.value(), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
                    break;
                }
            }
            reposition = false;
        }

        return resized;
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

        const auto window = ImGui::GetCurrentWindow();
        const auto viewport = ImGui::GetMainViewport();
        if (window->Viewport != viewport)
        {
            in_window_offset.reset();
        }
        else
        {
            const auto vpc = viewport->GetCenter();
            const auto pos = *last_position;

            if (pos.x <= vpc.x)
            {
                if (pos.y <= vpc.y)
                {
                    anchor = Anchor::TopLeft;
                }
                else
                {
                    anchor = Anchor::BottomLeft;
                    in_window_offset = *last_position - (viewport->Pos + ImVec2(0, viewport->Size.y));
                }
            }
            else
            {
                if (pos.y <= vpc.y)
                {
                    anchor = Anchor::TopRight;
                    in_window_offset = *last_position - (viewport->Pos + ImVec2(viewport->Size.x, 0));
                }
                else
                {
                    anchor = Anchor::BottomRight;
                    in_window_offset = *last_position - (viewport->Pos + viewport->Size);
                }
            }
        }
    }

    void ImGuiAnchor::record_size()
    {
        last_size = ImGui::GetWindowSize();
    }

    namespace
    {
        bool copy_button(const std::string& text, IClipboard& clipboard)
        {
            if (ImGui::Selectable(text.c_str()))
            {
                clipboard.write(to_utf16(text));
                return true;
            }
            return false;
        }
    }

    void read_only_colour_button(const std::string& name, const ImVec4& colour, IClipboard& clipboard)
    {
        const std::string popup_name = std::format("{}-picker", name);
        const bool open_popup = ImGui::ColorButton(name.c_str(), colour, 0, ImVec2(16, 16));
        if (open_popup)
        {
            ImGui::OpenPopup(popup_name.c_str());
        }

        if (ImGui::BeginPopup(popup_name.c_str()))
        {
            if (ImGui::Button("Copy as.."))
            {
                ImGui::OpenPopup("Copy");
            }

            bool close_popup = false;
            if (ImGui::BeginPopup("Copy"))
            {
                close_popup =
                    copy_button(std::format("#{:0>2X}{:0>2X}{:0>2X}", static_cast<int>(colour.x * 255.0f + 0.5f), static_cast<int>(colour.y * 255.0f + 0.5f), static_cast<int>(colour.z * 255.0f + 0.5f)), clipboard) ||
                    copy_button(std::format("{},{},{}", static_cast<int>(colour.x * 255.0f + 0.5f), static_cast<int>(colour.y * 255.0f + 0.5f), static_cast<int>(colour.z * 255.0f + 0.5f)), clipboard) ||
                    copy_button(std::format("{:.3f},{:.3f},{:.3f}", colour.x, colour.y, colour.z), clipboard);
                ImGui::EndPopup();
            }

            if (close_popup)
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
}
