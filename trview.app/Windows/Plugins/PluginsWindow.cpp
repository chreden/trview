#include "PluginsWindow.h"

namespace trview
{
    IPluginsWindow::~IPluginsWindow()
    {
    }

    PluginsWindow::PluginsWindow(const std::weak_ptr<IPlugins>& plugins, const std::shared_ptr<IShell>& shell)
        : _plugins(plugins), _shell(shell)
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
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            if (ImGui::BeginTable("Plugins", 4, ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Author");
                ImGui::TableSetupColumn("Location");
                ImGui::TableSetupColumn("Description");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                if (auto plugins = _plugins.lock())
                {
                    for (const auto& p : plugins->plugins())
                    {
                        if (auto plugin = p.lock())
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text(plugin->name().c_str());
                            ImGui::TableNextColumn();
                            ImGui::Text(plugin->author().c_str());
                            ImGui::TableNextColumn();
                            if (ImGui::Button(std::format("Open##{}", plugin->name()).c_str()))
                            {
                                _shell->open(to_utf16(plugin->path()));
                            }
                            ImGui::TableNextColumn();
                            ImGui::Text(plugin->description().c_str());
                        }
                    }
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();
        return stay_open;
    }

    void PluginsWindow::set_number(int32_t number)
    {
        _id = std::format("Plugins {}", number);
    }

    void PluginsWindow::update(float)
    {
    }
}
