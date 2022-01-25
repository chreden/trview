#include "GoTo.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <external/imgui/imgui.h>
#include <external/imgui/imgui_internal.h>

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
            auto viewport = ImGui::GetMainViewport();
            auto final = viewport->Pos + viewport->Size * 0.5f;
            ImGui::SetNextWindowPos(final, 0, ImVec2(0.5f, 0.5f));
            if (ImGui::Begin((std::string("Go To ") + to_utf8(_name)).c_str(), &_visible,
                ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
            {
                if (ImGui::IsWindowAppearing())
                {
                    ImGui::SetKeyboardFocusHere();
                }
                if (ImGui::InputInt("##gotoentry", &_index, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    on_selected(_index);
                    _visible = false;
                }
            }
            ImGui::End();
        }
    }
}
