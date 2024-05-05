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

    Console::Console(const std::shared_ptr<IDialogs>& dialogs, const std::weak_ptr<IPlugins>& plugins, const std::shared_ptr<IFonts>& fonts)
        : _dialogs(dialogs), _plugins(plugins), _fonts(fonts)
    {
        if (auto plugins_ptr = _plugins.lock())
        {
            for (const auto& plugin : plugins_ptr->plugins())
            {
                if (auto plugin_ptr = plugin.lock())
                {
                    auto raw = plugin_ptr.get();
                    _token_store += plugin_ptr->on_message += [raw, this](const std::string&)
                    {
                        _need_scroll.insert(raw);
                    };
                }
            }
        }
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
        const auto font = _fonts->font("console");
        if (font)
        {
            ImGui::PushFont(font);
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
                        if (auto plugin = _selected_plugin.lock())
                        {
                            plugin->do_file(escape(filename.value().filename));
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
                                if (auto plugin = _selected_plugin.lock())
                                {
                                    const std::string text = *iter;
                                    plugin->do_file(escape(text));
                                    std::erase(_recent_files, text);
                                    _recent_files.push_back(text);
                                }
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
                        if (auto plugin = _selected_plugin.lock())
                        {
                            plugin->clear_messages();
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            if (ImGui::BeginTabBar("TabBar"))
            {
                if (auto plugins = _plugins.lock())
                {
                    for (const auto& plugin : plugins->plugins())
                    {
                        if (auto p = plugin.lock())
                        {
                            render_plugin_logs(p);
                        }
                    }
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        if (font)
        {
            ImGui::PopFont();
        }
        return stay_open;
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

    void Console::render_plugin_logs(const std::shared_ptr<IPlugin>& plugin)
    {
        if (ImGui::BeginTabItem(plugin->name().c_str()))
        {
            _selected_plugin = plugin;

            auto text = plugin->messages();
            ImGui::InputTextMultiline(Names::log.c_str(), &text, ImVec2(-1, -25), ImGuiInputTextFlags_ReadOnly);
            if (ImGui::BeginChild(Names::log.c_str()))
            {
                if (need_scroll(plugin))
                {
                    ImGui::SetScrollHereY(1.0f);
                }
                ImGui::EndChild();
            }
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
                plugin->add_message(std::format("> {}", command));
                plugin->execute(command);
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
            ImGui::EndTabItem();
        }
    }

    bool Console::need_scroll(const std::shared_ptr<IPlugin>& plugin)
    {
        auto iter = std::ranges::find(_need_scroll, plugin.get());
        if (iter == _need_scroll.end())
        {
            return false;
        }
        _need_scroll.erase(*iter);
        return true;
    }
}