#include "Toolbar.h"

namespace trview
{
    IToolbar::~IToolbar()
    {
    }

    Toolbar::Toolbar(const std::weak_ptr<IPlugins>& plugins)
        : _plugins(plugins)
    {
    }

    void Toolbar::add_tool(const std::string& name)
    {
        _tools.push_back(name);
    }

    void Toolbar::render()
    {
        const auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + viewport->Size.y), 0, ImVec2(0.5f, 1.0f));
        if (ImGui::Begin("Toolbox", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize))
        {
            for (const auto& tool : _tools)
            {
                if (ImGui::Button(tool.c_str()))
                {
                    on_tool_clicked(tool);
                }
                ImGui::SameLine();
            }

            if (auto plugins = _plugins.lock())
            {
                for (const auto plugin_ptr : plugins->plugins())
                {
                    if (auto plugin = plugin_ptr.lock())
                    {
                        plugin->render_toolbar();
                    }
                    ImGui::SameLine();
                }
            }
        }
        ImGui::End();
    }
}