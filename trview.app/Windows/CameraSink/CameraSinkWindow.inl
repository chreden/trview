#pragma once

namespace trview
{
    template <typename T>
    void CameraSinkWindow::add_stat(const std::string& name, const T&& value)
    {
        const auto string_value = get_string(value);
        ImGui::TableNextColumn();
        ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav));
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Copy"))
            {
                _clipboard->write(to_utf16(string_value));
            }
            ImGui::EndPopup();
        }
        if (_tips.find(name) != _tips.end())
        {
            ImGui::BeginTooltip();
            ImGui::Text(_tips[name].c_str());
            ImGui::EndTooltip();
        }
        ImGui::TableNextColumn();
        ImGui::Text(string_value.c_str());
    }
}
