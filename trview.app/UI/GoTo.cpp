#include "GoTo.h"
#include <charconv>

namespace trview
{
    bool GoTo::visible() const
    {
        return _visible;
    }

    void GoTo::toggle_visible()
    {
        _visible = !_visible;
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

                std::string current_input = _current_input;
                ImGui::InputText("##gotoentry", &current_input);
                _current_input = current_input;

                bool any_selected = false;
                if (!_current_input.empty())
                {
                    const auto make_upper = std::views::transform([](auto&& c) { return static_cast<char>(std::toupper(c)); }) | std::ranges::to<std::string>();
                    const auto upper_input = _current_input | make_upper;
                    std::optional<uint32_t> numeric_input;
                    {
                        uint32_t temp_numeric_input;
                        if (std::from_chars(upper_input.data(), upper_input.data() + upper_input.size(), temp_numeric_input).ec == std::errc {})
                        {
                            numeric_input = temp_numeric_input;
                        }
                    }
                    const auto search_results = _items
                        | std::views::filter([&](auto&& i) { return (numeric_input && i.number == numeric_input.value()) || (i.name | make_upper).contains(upper_input); })
                        | std::ranges::to<std::vector>();

                    for (const auto& item : search_results)
                    {
                        const auto item_id = std::format("{} - {}", item.number, item.name);
                        if (ImGui::Selectable(item_id.c_str(), false, ImGuiSelectableFlags_DontClosePopups | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                        {
                            on_selected(item.number);
                        }
                        any_selected |= ImGui::GetCurrentContext()->NavId == ImGui::GetCurrentWindow()->GetID(item_id.c_str());
                    }
                }

                ImGui::EndPopup();

                if (ImGui::IsKeyPressed(ImGuiKey_Escape) || (any_selected && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))))
                {
                    _visible = false;
                    ImGui::FocusWindow(nullptr);
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
