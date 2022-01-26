#include "Toolbar.h"

namespace trview
{
    void Toolbar::add_tool(const std::wstring& name, const std::wstring& text)
    {
        _tools.push_back(to_utf8(name));
    }

    void Toolbar::render()
    {
        const auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + viewport->Size.y), 0, ImVec2(0.5f, 1.0f));
        if (ImGui::Begin("Toolbox", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize))
        {
            for (const auto& tool : _tools)
            {
                if (ImGui::Button(tool.c_str()))
                {
                    on_tool_clicked(to_utf16(tool));
                }
                ImGui::SameLine();
            }
        }
        ImGui::End();
    }
}