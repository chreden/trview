#include "LogWindow.h"
#include <format>

namespace trview
{
    ILogWindow::~ILogWindow()
    {
    }

    LogWindow::LogWindow(const std::shared_ptr<ILog>& log)
        : _log(log)
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
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(520, 400));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            // One tab for each topic + all
            if (ImGui::BeginTabBar("topics", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("All"))
                {
                    if (ImGui::BeginChild("allmessages"))
                    {
                        for (const auto& message : _log->messages())
                        {
                            std::string activities;
                            for (const auto& activity : message.activity)
                            {
                                activities += "[" + activity + "]";
                            }

                            ImGui::Text("[%s] [%s] %s - %s", message.topic, message.timestamp.c_str(), activities.c_str(), message.text.c_str());
                        }
                    }
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }

                for (const auto& topic : _log->topics())
                {
                    if (ImGui::BeginTabItem(topic.c_str()))
                    {
                        if (ImGui::BeginTabBar((topic + "-activities").c_str(), ImGuiTabBarFlags_None))
                        {
                            for (const auto& activity : _log->activities(topic))
                            {
                                if (ImGui::BeginTabItem(activity.c_str()))
                                {
                                    if (ImGui::BeginChild((topic + "-" + activity).c_str()))
                                    {
                                        for (const auto& message : _log->messages(topic, activity))
                                        {
                                            std::string activities;
                                            for (auto iter = message.activity.begin() + 1; iter != message.activity.end(); ++iter)
                                            {
                                                activities += "[" + *iter + "]";
                                            }

                                            auto get_colour = [&]()
                                            {
                                                switch (message.status)
                                                {
                                                case Message::Status::Warning:
                                                    return ImVec4(1, 1, 0, 1);
                                                case Message::Status::Error:
                                                    return ImVec4(1, 0, 0, 1);
                                                }
                                                return ImVec4(1, 1, 1, 1);
                                            };

                                            ImGui::PushStyleColor(ImGuiCol_Text, get_colour());
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
}

