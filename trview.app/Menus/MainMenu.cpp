#include "MainMenu.h"
#include "IFileMenu.h"
#include "IViewMenu.h"
#include "../Messages/Messages.h"
#include "../UI/ILevelInfo.h"

namespace trview
{
    IMainMenu::~IMainMenu()
    {
    }

    MainMenu::MainMenu(const std::weak_ptr<IMessageSystem>& messaging, const std::shared_ptr<IFileMenu>& file_menu, const std::shared_ptr<IViewMenu>& view_menu,
        const std::shared_ptr<ILevelInfo>& level_info)
        : _messaging(messaging), _file_menu(file_menu), _view_menu(view_menu), _level_info(level_info)
    {
        // TODO: Move all to messaging?
        _token_store += _file_menu->on_reload += []() {};
        _token_store += _file_menu->on_file_open += [&](auto&& file) { messages::send_open_level_filename(_messaging, file); };
    }

    void MainMenu::render()
    {
        if (ImGui::BeginMainMenuBar())
        {
            _file_menu->render();
            _view_menu->render();

            if (ImGui::BeginMenu("Windows"))
            {
                if (ImGui::MenuItem("Items", "Ctrl+I"))
                {
                    messages::commands::send_create_window(_messaging, "Items");
                }

                if (ImGui::MenuItem("Triggers", "Ctrl+T"))
                {
                    messages::commands::send_create_window(_messaging, "Triggers");
                }

                if (ImGui::MenuItem("Rooms", "Ctrl+M"))
                {
                    messages::commands::send_create_window(_messaging, "Rooms");
                }

                if (ImGui::MenuItem("Route", "Ctrl+R"))
                {
                    messages::commands::send_create_window(_messaging, "Route");
                }

                if (ImGui::MenuItem("Lights", "Ctrl+L"))
                {
                    messages::commands::send_create_window(_messaging, "Lights");
                }

                if (ImGui::MenuItem("Camera/Sink", "Ctrl+K"))
                {
                    messages::commands::send_create_window(_messaging, "CameraSink");
                }

                if (ImGui::MenuItem("Plugins", "Ctrl+P"))
                {
                    messages::commands::send_create_window(_messaging, "Plugins");
                }

                if (ImGui::MenuItem("Sounds", "Ctrl+S"))
                {
                    messages::commands::send_create_window(_messaging, "Sounds");
                }

                if (ImGui::MenuItem("Diff", "Ctrl+D"))
                {
                    messages::commands::send_create_window(_messaging, "Diff");
                }

                if (ImGui::MenuItem("Pack", "Ctrl+P"))
                {
                    messages::commands::send_create_window(_messaging, "Pack");
                }

                if (ImGui::MenuItem("Console", "F9"))
                {
                    messages::commands::send_create_window(_messaging, "Console");
                }

                if (ImGui::MenuItem("Statics"))
                {
                    messages::commands::send_create_window(_messaging, "Statics");
                }

                if (ImGui::MenuItem("Log"))
                {
                    messages::commands::send_create_window(_messaging, "Log");
                }

                if (ImGui::MenuItem("Textures"))
                {
                    messages::commands::send_create_window(_messaging, "Textures");
                }

                if (ImGui::MenuItem("Camera Position"))
                {
                    messages::commands::send_show(_messaging, { .name = "camera_position", .value = true });
                }

                if (ImGui::MenuItem("Reset Layout"))
                {
                    messages::commands::send_reset_layout(_messaging);
                }

                if (ImGui::MenuItem("Reset Fonts"))
                {
                    messages::commands::send_reset_fonts(_messaging);
                }

                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Settings"))
            {
                messages::commands::send_toggle_settings(_messaging);
            }

            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("GitHub"))
                {
                    ShellExecute(0, 0, L"https://github.com/chreden/trview", 0, 0, SW_SHOW);
                }

                if (ImGui::MenuItem("Discord"))
                {
                    ShellExecute(0, 0, L"https://discord.gg/Zy7kYge", 0, 0, SW_SHOW);
                }

                if (ImGui::MenuItem("About"))
                {
                    messages::commands::send_create_window(_messaging, "About");
                }

                ImGui::EndMenu();
            }

            _level_info->render();
            ImGui::EndMainMenuBar();
        }
    }
}
