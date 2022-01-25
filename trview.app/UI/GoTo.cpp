#include "GoTo.h"
#include <external/imgui/imgui.h>

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
            if (ImGui::Begin((std::string("Go To ") + to_utf8(_name)).c_str(), &_visible, ImGuiWindowFlags_AlwaysAutoResize))
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
