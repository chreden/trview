#include "PluginsWindow.h"

namespace trview
{
    IPluginsWindow::~IPluginsWindow()
    {
    }

    PluginsWindow::PluginsWindow(const std::weak_ptr<IPlugins>& plugins)
        : _plugins(plugins)
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
            ImGui::Text("Plugins content here");
        }
        ImGui::End();
        return stay_open;
    }

    void PluginsWindow::set_number(int32_t number)
    {
        _id = std::format("Plugins {}", number);
    }
}
