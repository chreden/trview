#include "Console.h"
#include <external/imgui/imgui_internal.h>

namespace trview
{
    namespace
    {
        std::string escape(const std::string& value)
        {
            std::string escaped = value;
            std::replace(escaped.begin(), escaped.end(), '\\', '/');
            return escaped;
        }
    }

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
                        const auto filename = _dialogs->open_file(L"Open Lua file", { { L"Lua files", { L"*.lua" } } }, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST);
                        if (filename)
                        {
                            on_command(std::format("dofile(\"{}\")", escape(filename.value().filename)));
                            std::erase(_recent_files, filename.value().filename);
                            _recent_files.push_back(filename.value().filename);
                        }
                    }

                    if (ImGui::BeginMenu("Open Recent", !_recent_files.empty()))
                    {
                        for (auto iter = _recent_files.rbegin(); iter != _recent_files.rend(); ++iter)
                        {
                            if (ImGui::MenuItem(iter->c_str()))
                            {
                                const std::string text = *iter;
                                on_command(std::format("dofile(\"{}\")", escape(text)));
                                std::erase(_recent_files, text);
                                _recent_files.push_back(text);
                                break;
                            }
                        }

                        ImGui::EndMenu();
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

    void Console::set_font(ImFont* font)
    {
        _font = font;
    }

    void Console::set_number(int32_t number)
    {
        _id = "Console " + std::to_string(number);
    }
}