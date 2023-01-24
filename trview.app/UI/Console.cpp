#include "Console.h"

namespace trview
{
    Console::Console(const std::shared_ptr<IFiles>& files)
        : _files(files)
    {
    }

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
            if (_font)
            {
                ImGui::PushFont(_font);
            }
            ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(520, 400));
            if (ImGui::Begin("Console", &_visible))
            {
                ImGui::InputTextMultiline(Names::log.c_str(), const_cast<char*>(_text.c_str()), _text.size(), ImVec2(-1, -25), ImGuiInputTextFlags_ReadOnly);
                if (ImGui::IsWindowAppearing() || _need_focus)
                {
                    ImGui::SetKeyboardFocusHere();
                    _need_focus = false;
                }
                ImGui::PushItemWidth(-1);
                if (ImGui::InputText(Names::input.c_str(), &_buffer[0], _buffer.size(), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    _need_focus = true;
                    auto command = std::string(_buffer.data());
                    on_command(command);
                    std::ranges::fill(_buffer, '\0');
                }
                ImGui::PopItemWidth();
            }
            ImGui::End();
            ImGui::PopStyleVar();
            if (_font)
            {
                ImGui::PopFont();
            }
        }
    }

    void Console::initialise_ui()
    {
        auto context = ImGui::GetCurrentContext();
        if (context)
        {
            _font = context->IO.Fonts->AddFontFromFileTTF((_files->fonts_directory() + "\\Consola.ttf").c_str(), 14.0f);
        }
    }
}