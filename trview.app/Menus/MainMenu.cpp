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

        _token_store += _file_menu->on_exit += []() {};
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
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }
}
