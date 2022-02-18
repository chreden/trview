#include "Console.h"

namespace trview
{
    bool Console::visible() const
    {
        return _visible;
    }

    void Console::print(const std::wstring& text)
    {
        if (!_text.empty())
        {
            _text += '\n';
        }
        _text += to_utf8(text);
    }

    void Console::set_visible(bool value)
    {
        _visible = value;;
    }

    void Console::render()
    {
        if (_visible)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(520, 400));
            if (ImGui::Begin("Console", &_visible))
            {
                ImGui::InputTextMultiline("##Log", const_cast<char*>(_text.c_str()), _text.size(), ImVec2(-1, -25), ImGuiInputTextFlags_ReadOnly);
                if (ImGui::IsWindowAppearing())
                {
                    ImGui::SetKeyboardFocusHere();
                }
                std::vector<char> vec;
                vec.resize(512);
                ImGui::PushItemWidth(-1);
                if (ImGui::InputText("##input", &vec[0], vec.size(), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    auto command = std::string(&vec[0]);
                    on_command(to_utf16(command));
                }
                ImGui::PopItemWidth();
            }
            ImGui::End();
            ImGui::PopStyleVar();
        }
    }
}