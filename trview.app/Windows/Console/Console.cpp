#include "Console.h"

namespace trview
{
    IConsole::~IConsole()
    {
    }

    Console::Console(const std::shared_ptr<IFiles>& files, const std::shared_ptr<IDialogs>& dialogs)
        : _files(files), _dialogs(dialogs)
    {
    }

    void Console::print(const std::string& text)
    {
        if (!_text.empty())
        {
            _text += '\n';
        }
        _text += text;
    }

    void Console::render()
    {
        if (!render_console())
        {
            on_window_closed();
            return;
        }
    }

    bool Console::render_console()
    {
        bool stay_open = true;
        if (_font)
        {
            ImGui::PushFont(_font);
        }
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(520, 400));
        if (ImGui::Begin(_id.c_str(), &stay_open, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open"))
                    {

                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Edit"))
                {
                    if (ImGui::MenuItem("Clear"))
                    {
                        _text.clear();
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

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
        return stay_open;
    }

    void Console::initialise_ui()
    {
        auto context = ImGui::GetCurrentContext();
        if (context)
        {
            _font = context->IO.Fonts->AddFontFromFileTTF((_files->fonts_directory() + "\\Consola.ttf").c_str(), 14.0f);
        }
    }

    void Console::set_number(int32_t number)
    {
        _id = "Console " + std::to_string(number);
    }
}