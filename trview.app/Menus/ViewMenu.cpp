#include "ViewMenu.h"
#include "../Messages/Messages.h"

namespace trview
{
    IViewMenu::~IViewMenu()
    {
    }

    ViewMenu::ViewMenu(const std::weak_ptr<IMessageSystem>& messaging)
        : _messaging(messaging)
    {
    }

    void ViewMenu::render()
    {
        if (ImGui::Shortcut(ImGuiMod_Alt | ImGuiKey_V, 0U, ImGuiInputFlags_RouteGlobal))
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            const ImGuiID id = window->GetID("View");
            ImGui::OpenPopup(id, ImGuiPopupFlags_None);
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("UI", nullptr, &_show_ui))
            {
                if (!_show_ui)
                {
                    show("minimap", false);
                    show("tooltip", false);
                    show("compass", false);
                    show("selection", false);
                    show("route", false);
                    show("tools", false);
                }
                else
                {
                    // Return the states of the settings to what they were before the main UI toggle was changed.
                    show("minimap", _show_minimap);
                    show("tooltip", _show_tooltip);
                    show("compass", _show_compass);
                    show("selection", _show_selection);
                    show("route", _show_route);
                    show("tools", _show_tools);
                }
                show("ui", _show_ui);
            }

            if (ImGui::MenuItem("Minimap", nullptr, &_show_minimap, _show_ui))
            {
                show("minimap", _show_minimap);
            }

            if (ImGui::MenuItem("Tooltip", nullptr, &_show_tooltip, _show_ui))
            {
                show("tooltip", _show_tooltip);
            }

            if (ImGui::MenuItem("Compass", nullptr, &_show_compass, _show_ui))
            {
                show("compass", _show_compass);
            }

            if (ImGui::MenuItem("Selection", nullptr, &_show_selection, _show_ui))
            {
                show("selection", _show_selection);
            }

            if (ImGui::MenuItem("Route", nullptr, &_show_route, _show_ui))
            {
                show("route", _show_route);
            }

            if (ImGui::MenuItem("Tools", nullptr, &_show_tools, _show_ui))
            {
                show("tools", _show_tools);
            }

            if (ImGui::MenuItem("Unhide All"))
            {
                messages::commands::send_unhide_all(_messaging);
            }

            ImGui::EndMenu();
        }
    }

    void ViewMenu::show(const std::string& key, bool value)
    {
        messages::commands::send_show(_messaging, { .name = key, .value = value });
    }
}
