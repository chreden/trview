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

    Console::Console(const std::shared_ptr<IDialogs>& dialogs)
        : _dialogs(dialogs)
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

    int Console::callback(ImGuiInputTextCallbackData* data)
    {
        auto instance = static_cast<Console*>(data->UserData);
        if (instance->_go_to_eol)
        {
            data->CursorPos = static_cast<int32_t>(instance->_buffer.size());
            instance->_go_to_eol = false;
        }
        return 0;
    }

    bool Console::render_console()
    {
        bool stay_open = true;
        if (_font)
        {
            ImGui::PushFont(_font);
        }
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(520, 400));
        if (ImGui::Begin(_id.c_str(), &stay_open, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoNavInputs))
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
            if (ImGui::InputText(Names::input.c_str(), &_buffer, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackAlways, callback, this))
            { 
                _need_focus = true;
                auto command = std::string(_buffer.data());
                print(std::format("> {}", command));
                on_command(command);
                add_command(command);
                _buffer.clear();
            }
            ImGui::PopItemWidth();

            bool focused = ImGui::IsItemFocused();
            if (focused)
            {
                if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
                {
                    if (--_command_history_index >= 0)
                    {
                        _buffer = _command_history[_command_history_index];
                    }
                    else
                    {
                        _command_history_index = -1;
                        _buffer.clear();
                    }
                    _need_focus = true;
                    _go_to_eol = true;
                    ImGui::ClearActiveID();
                }

                if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
                {
                    if (++_command_history_index < static_cast<int32_t>(_command_history.size()))
                    {
                        _buffer = _command_history[_command_history_index];
                    }
                    else
                    {
                        _command_history_index = static_cast<int32_t>(_command_history.size());
                        _buffer.clear();
                    }
                    _need_focus = true;
                    _go_to_eol = true;
                    ImGui::ClearActiveID();
                }
            }
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

    void Console::add_command(const std::string& command)
    {
        auto existing = std::ranges::find(_command_history, command);
        if (existing != _command_history.end())
        {
            _command_history.erase(existing);
        }
        _command_history.push_back(command);
        _command_history_index = static_cast<int32_t>(_command_history.size());
    }
}