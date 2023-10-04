#include "GoTo.h"

namespace trview
{
    bool GoTo::visible() const
    {
        return _visible;
    }

    void GoTo::toggle_visible(int32_t value)
    {
        _visible = !_visible;
        _index = value;
        _shown = false;
        _current_input.clear();
    }

    std::string GoTo::name() const
    {
        return _name;
    }

    void GoTo::set_name(const std::string& name)
    {
        _name = name;
    }

    void GoTo::set_items(const std::vector<GoToItem>& items)
    {
        _items = items;
    }

    void GoTo::render()
    {
        if (_visible)
        {
            const auto viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos + viewport->Size * 0.5f, 0, ImVec2(0.5f, 0.5f));

            const std::string id = std::format("Go To {}", _name);
            if (!_shown)
            {
                ImGui::OpenPopup(id.c_str());
                _shown = true;
            }
            if (ImGui::BeginPopup(id.c_str(), 0))
            {
                ImGui::Text(id.c_str());
                if (ImGui::IsWindowAppearing())
                {
                    ImGui::SetKeyboardFocusHere();
                }

                if (!_items.empty() && false)
                {
                    std::string current_input = _current_input;
                    ImGui::InputText("##gotoentry", &current_input);
                    _current_input = current_input;

                    if (!_current_input.empty())
                    {
                        const auto search_results = _items
                            | std::views::filter([&](auto&& i) { return i.name.starts_with(_current_input); })
                            | std::ranges::to<std::vector>();

                        for (const auto& item : search_results)
                        {
                            if (ImGui::Selectable(std::format("{} - {}", item.number, item.name).c_str(), false, ImGuiSelectableFlags_DontClosePopups | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                            {
                                on_selected(item.number);
                            }
                        }
                    }

                    ImGui::EndPopup();

                    if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))
                    {
                        _visible = false;
                        ImGui::FocusWindow(nullptr);
                    }
                }
                else
                {
                    if (ImGui::InputInt("##gotoentry", &_index, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        if (_index >= 0)
                        {
                            on_selected(_index);
                        }
                    }

                    ImGui::EndPopup();

                    if (ImGui::IsKeyPressed(ImGuiKey_Escape) || ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))
                    {
                        if (!ImGui::IsKeyPressed(ImGuiKey_Escape) && _index >= 0)
                        {
                            on_selected(_index);
                        }
                        _visible = false;
                        ImGui::FocusWindow(nullptr);
                    }
                }
            }
            else
            {
                _visible = false;
                ImGui::FocusWindow(nullptr);
            }
        }
    }
}
