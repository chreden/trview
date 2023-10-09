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

    void GoTo::set_items(const std::vector<GoToItem>& items)
    {
        _items = items;
    }

    void GoTo::render()
    {
        if (_visible)
        {
            const auto viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos + ImVec2(viewport->Size.x * 0.5f, viewport->Size.y * 0.25f), 0, ImVec2(0.5f, 0.5f));

            const std::string id = "Find";
            if (!_shown)
            {
                ImGui::OpenPopup(id.c_str());
                _shown = true;
            }
            if (ImGui::BeginPopup(id.c_str(), ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text(id.c_str());
                if (ImGui::IsWindowAppearing() || _need_focus)
                {
                    ImGui::SetKeyboardFocusHere();
                    _need_focus = false;
                }

                std::string current_input = _current_input;
                ImGui::InputText("##gotoentry", &current_input);
                _current_input = current_input;

                auto outer_window = ImGui::GetCurrentWindow();

                bool need_list_focus = false;
                if (ImGui::GetCurrentContext()->NavId == ImGui::GetCurrentWindow()->GetID("##gotoentry"))
                {
                    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
                    {
                        need_list_focus = true;
                    }
                    else if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
                    {
                        ImGui::NavMoveRequestCancel();
                    }
                }

                bool any_selected = false;
                if (ImGui::BeginChild("##searchresults", ImVec2(-1, 100)))
                {
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

                        bool list_focused = false;
                        bool first_item = true;

                        if (need_list_focus)
                        {
                            ImGui::SetKeyboardFocusHere();
                            need_list_focus = false;
                        }

                        for (const auto& item : search_results)
                        {
                            if (!list_focused && ImGui::GetCurrentContext()->NavId == outer_window->GetID("##searchresults"))
                            {
                                ImGui::SetKeyboardFocusHere();
                                list_focused = true;
                            }

                            const auto item_id = std::format("{} - {}", item.number, item.name);
                            if (first_item &&
                                ImGui::GetCurrentContext()->NavId == ImGui::GetCurrentWindow()->GetID(item_id.c_str()) &&
                                ImGui::IsKeyPressed(ImGuiKey_UpArrow))
                            {
                                _need_focus = true;
                            }
                            first_item = false;

                            if (ImGui::Selectable(item_id.c_str(), false, ImGuiSelectableFlags_DontClosePopups | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                            {
                                on_selected(item);
                            }

                            any_selected |= ImGui::GetCurrentContext()->NavId == ImGui::GetCurrentWindow()->GetID(item_id.c_str());
                        }
                    }
                }
                ImGui::EndChild();
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
