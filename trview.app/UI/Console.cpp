#include "Console.h"

namespace trview
{
    bool Console::visible() const
    {
        return _visible;
    }

    void Console::print(const std::string& text)
    {
        if (!_text.empty())
        {
            _text += '\n';
        }
        _text += text;
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
                ImGui::InputTextMultiline(Names::log.c_str(), const_cast<char*>(_text.c_str()), _text.size(), ImVec2(-1, -25), ImGuiInputTextFlags_ReadOnly);
                if (ImGui::IsWindowAppearing() || _need_focus)
                {
                    ImGui::SetKeyboardFocusHere();
                    _need_focus = false;
                }
                std::vector<char> vec;
                vec.resize(512);
                ImGui::PushItemWidth(-1);
                if (ImGui::InputText(Names::input.c_str(), &vec[0], vec.size(), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    _need_focus = true;
                    auto command = std::string(&vec[0]);
                    on_command(command);
                }
                ImGui::PopItemWidth();
            }
            ImGui::End();
            ImGui::PopStyleVar();
        }
    }
}