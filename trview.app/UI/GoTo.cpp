#include "GoTo.h"

namespace trview
{
    bool GoTo::visible() const
    {
        return _visible;
    }

    void GoTo::toggle_visible()
    {
        _visible = !_visible;
        _index = 0;
        _shown = false;
    }

    std::wstring GoTo::name() const
    {
        return _name;
    }

    void GoTo::set_name(const std::wstring& name)
    {
        _name = name;
    }

    void GoTo::render()
    {
        if (_visible)
        {
            const auto viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos + viewport->Size * 0.5f, 0, ImVec2(0.5f, 0.5f));

            const std::string id = (std::string("Go To ") + to_utf8(_name));
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
                if(ImGui::InputInt("##gotoentry", &_index, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    on_selected(_index);
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
                _visible = false;
                ImGui::FocusWindow(nullptr);
            }
        }
    }
}
