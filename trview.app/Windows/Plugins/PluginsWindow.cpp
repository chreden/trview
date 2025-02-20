#include "PluginsWindow.h"

namespace trview
{
    IPluginsWindow::~IPluginsWindow()
    {
    }

    PluginsWindow::PluginsWindow(const std::weak_ptr<IPlugins>& plugins, const std::shared_ptr<IShell>& shell, const std::shared_ptr<IDialogs>& dialogs)
        : _plugins(plugins), _shell(shell), _dialogs(dialogs)
    {
    }

    void PluginsWindow::render()
    {
        if (!render_plugins_window())
        {
            on_window_closed();
            return;
        }
    }

    bool PluginsWindow::render_plugins_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(540, 500));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            if (auto plugins = _plugins.lock())
            {
                ImGui::Text("Plugin Directories");
                if (ImGui::BeginTable("Directories", 3, ImGuiTableFlags_SizingFixedFit, ImVec2(0, 0)))
                {
                    const auto directories = _settings.plugin_directories;
                    for (std::size_t i = 0; i < directories.size(); ++i)
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();

                        if (ImGui::Button(std::format("Remove##{}", i).c_str()))
                        {
                            _settings.plugin_directories.erase(_settings.plugin_directories.begin() + i);
                            on_settings(_settings);
                            plugins->reload();
                        }
                        ImGui::TableNextColumn();
                        if (ImGui::Button(std::format("Open##{}", i).c_str()))
                        {
                            _shell->open(to_utf16(directories[i]));
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(directories[i].c_str());
                    }

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    if (ImGui::Button("+"))
                    {
                        if (auto path = _dialogs->open_folder())
                        {
                            _settings.plugin_directories.push_back(path.value());
                            on_settings(_settings);
                            plugins->reload();
                        }
                    }

                    ImGui::EndTable();
                }

                ImGui::Text("Plugins");
                if (ImGui::BeginTable(Names::plugins_list.c_str(), 6, ImGuiTableFlags_SizingStretchProp))
                {
                    ImGui::TableSetupColumn("Enabled");
                    ImGui::TableSetupColumn("Location");
                    ImGui::TableSetupColumn("Action");
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Author");
                    ImGui::TableSetupColumn("Description");
                    ImGui::TableSetupScrollFreeze(1, 1);
                    ImGui::TableHeadersRow();

                    for (const auto& p : plugins->plugins())
                    {
                        if (auto plugin = p.lock())
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            bool enabled = plugin->enabled();
                            ImGui::BeginDisabled(plugin->built_in());
                            if (ImGui::Checkbox(std::format("##enabled-{}", reinterpret_cast<std::size_t>(plugin.get())).c_str(), &enabled))
                            {
                                plugin->set_enabled(enabled);
                                _settings.plugins[plugin->path()].enabled = enabled;
                                on_settings(_settings);
                            }
                            ImGui::EndDisabled();
                            ImGui::TableNextColumn();
                            if (!plugin->path().empty() && ImGui::Button(std::format("Open##{}", plugin->name()).c_str()))
                            {
                                _shell->open(to_utf16(plugin->path()));
                            }
                            ImGui::TableNextColumn();
                            if (!plugin->path().empty() && ImGui::Button(std::format("Reload##{}", plugin->name()).c_str()))
                            {
                                plugin->reload();
                            }
                            ImGui::TableNextColumn();
                            ImGui::Text(plugin->name().c_str());
                            ImGui::TableNextColumn();
                            ImGui::Text(plugin->author().c_str());
                            ImGui::TableNextColumn();
                            ImGui::Text(plugin->description().c_str());
                        }
                    }

                    ImGui::EndTable();
                }
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    void PluginsWindow::set_number(int32_t number)
    {
        _id = std::format("Plugins {}", number);
    }

    void PluginsWindow::set_settings(const UserSettings& settings)
    {
        _settings = settings;
    }

    void PluginsWindow::update(float)
    {
    }
}
