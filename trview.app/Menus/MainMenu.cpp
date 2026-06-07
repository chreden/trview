#include "MainMenu.h"
#include "IFileMenu.h"
#include "IViewMenu.h"
#include "../Messages/Messages.h"

namespace trview
{
    IMainMenu::~IMainMenu()
    {
    }

    MainMenu::MainMenu(const std::weak_ptr<IMessageSystem>& messaging, const std::shared_ptr<IFileMenu>& file_menu, const std::shared_ptr<IViewMenu>& view_menu)
        : _messaging(messaging), _file_menu(file_menu), _view_menu(view_menu)
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

                }

                if (ImGui::MenuItem("Triggers", "Ctrl+T"))
                {

                }

                if (ImGui::MenuItem("Rooms", "Ctrl+M"))
                {

                }

                if (ImGui::MenuItem("Route", "Ctrl+R"))
                {

                }

                if (ImGui::MenuItem("Lights", "Ctrl+L"))
                {

                }

                if (ImGui::MenuItem("Camera/Sink", "Ctrl+K"))
                {

                }

                if (ImGui::MenuItem("Log"))
                {

                }

                if (ImGui::MenuItem("Textures"))
                {

                }

                if (ImGui::MenuItem("Log"))
                {

                }

                if (ImGui::MenuItem("Console", "F9"))
                {

                }

                if (ImGui::MenuItem("Plugins", "Ctrl+P"))
                {

                }

                if (ImGui::MenuItem("Statics", "Ctrl+S"))
                {

                }

                if (ImGui::MenuItem("Sounds", "Ctrl+S"))
                {

                }

                if (ImGui::MenuItem("Diff", "Ctrl+D"))
                {

                }

                if (ImGui::MenuItem("Pack", "Ctrl+D"))
                {

                }

                if (ImGui::MenuItem("Camera Position"))
                {

                }

                if (ImGui::MenuItem("Reset Layout"))
                {

                }

                if (ImGui::MenuItem("Reset Fonts"))
                {

                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("GitHub"))
                {

                }

                if (ImGui::MenuItem("Discord"))
                {

                }

                if (ImGui::MenuItem("About"))
                {

                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }
}
