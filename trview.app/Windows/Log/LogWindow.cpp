#include "LogWindow.h"
#include <format>
#include <sstream>

namespace trview
{
    LogWindow::LogWindow(const std::shared_ptr<ILog>& log, const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IFiles>& files)
        : _log(log), _dialogs(dialogs), _files(files)
    {
    }

    void LogWindow::update(float)
    {
    }

    void LogWindow::render()
    {
        if (!render_log_window())
        {
            on_window_closed();
            return;
        }
    }

    bool LogWindow::render_log_window()
    {
        const auto get_colour = [&](auto&& message)
        {
            switch (message.status)
            {
            case LogMessage::Status::Warning:
                return ImVec4(1, 1, 0, 1);
            case LogMessage::Status::Error:
                return ImVec4(1, 0, 0, 1);
            }
            return ImVec4(1, 1, 1, 1);
        };

        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(520, 400));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            // One tab for each topic + all
            if (ImGui::BeginTabBar(Names::topics_tabs.c_str(), ImGuiTabBarFlags_FittingPolicyScroll))
            {
                if (ImGui::BeginTabItem(Names::all_topic.c_str()))
                {
                    if (ImGui::Button(Names::save.c_str()))
                    {
                        save_to_file(_log->messages(), 0);
                    }

                    if (ImGui::BeginChild("allmessages", ImVec2(), false, ImGuiWindowFlags_HorizontalScrollbar))
                    {
                        for (const auto& message : _log->messages())
                        {
                            std::string activities;
                            for (const auto& activity : message.activity)
                            {
                                activities += "[" + activity + "]";
                            }

                            ImGui::PushStyleColor(ImGuiCol_Text, get_colour(message));
                            ImGui::Text("[%s] [%s] %s - %s", message.topic.c_str(), message.timestamp.c_str(), activities.c_str(), message.text.c_str());
                            ImGui::PopStyleColor();
                        }
                    }
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }

                for (const auto& topic : _log->topics())
                {
                    if (ImGui::BeginTabItem(topic.c_str()))
                    {
                        if (ImGui::BeginTabBar((topic + "-activities").c_str(), ImGuiTabBarFlags_FittingPolicyScroll))
                        {
                            for (const auto& activity : _log->activities(topic))
                            {
                                if (ImGui::BeginTabItem(activity.c_str()))
                                {
                                    if (ImGui::Button("Save"))
                                    {
                                        save_to_file(_log->messages(topic, activity), 1);
                                    }

                                    ImGui::Separator();
                                    if (ImGui::BeginChild((topic + "-" + activity).c_str(), ImVec2(), false, ImGuiWindowFlags_HorizontalScrollbar))
                                    {
                                        for (const auto& message : _log->messages(topic, activity))
                                        {
                                            std::string activities;
                                            for (auto iter = message.activity.begin() + 1; iter != message.activity.end(); ++iter)
                                            {
                                                activities += "[" + *iter + "]";
                                            }

                                            ImGui::PushStyleColor(ImGuiCol_Text, get_colour(message));
                                            ImGui::Text("[%s] %s - %s", message.timestamp.c_str(), activities.c_str(), message.text.c_str());
                                            ImGui::PopStyleColor();
                                        }
                                    }
                                    ImGui::EndChild();
                                    ImGui::EndTabItem();
                                }
                            }
                            ImGui::EndTabBar();
                        }
                        ImGui::EndTabItem();
                    }
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    void LogWindow::set_number(int32_t number)
    {
        _id = std::format("Log {}", number);
    }

    void LogWindow::save_to_file(const std::vector<LogMessage>& messages, int level_offset)
    {
        auto result = _dialogs->save_file(L"Save log", { { L"Log File", { L"*.txt" } } }, 1);
        if (!result.has_value())
        {
            return;
        }

        std::stringstream stream;
        std::for_each(messages.begin(), messages.end(),
            [&stream, level_offset](auto&& message)
            {
                std::string activities;
                for (uint32_t i = level_offset; i < message.activity.size(); ++i)
                {
                    activities += "[" + message.activity[i] + "]";
                }
                stream << std::format("[{}] {} - {}", message.timestamp, activities, message.text) << '\n';
            });
        _files->save_file(result.value().filename, stream.str());
    }

    std::string LogWindow::type() const
    {
        return "Log";
    }

    void LogWindow::receive_message(const Message&)
    {
    }

    std::string LogWindow::title() const
    {
        return _id;
    }
}

