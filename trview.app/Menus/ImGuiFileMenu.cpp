#include "ImGuiFileMenu.h"
#include <trview.common/Strings.h>
#include "../Messages/Messages.h"
#include "../Settings/UserSettings.h"

namespace trview
{
    ImGuiFileMenu::ImGuiFileMenu(const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IFiles>& files)
        : _dialogs(dialogs), _files(files)
    {
    }

    std::vector<std::string> ImGuiFileMenu::local_levels() const
    {
        return {};
    }

    void ImGuiFileMenu::open_file(const std::string& filename, const std::weak_ptr<trlevel::IPack>& pack)
    {
        if (const auto pack_ptr = pack.lock())
        {
            _file_switcher = valid_pack_levels(*pack_ptr);
        }
        else
        {
            _file_switcher = _files->get_files(path_for_filename(filename), default_file_pattern);
        }
    }

    void ImGuiFileMenu::render()
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open"))
            {
                const auto filename = _dialogs->open_file(L"Open level", { { L"All Tomb Raider Files", { L"*.tr2", L"*.tr4", L"*.trc", L"*.phd", L"*.psx", L"*.obj", L"*.tom", L"*.sat" } } }, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, _initial_directory);
                if (filename.has_value())
                {
                    _initial_directory = filename->directory;
                    on_file_open(filename->filename);
                }
            }

            if (ImGui::BeginMenu("Open Recent", !_recent_files.empty()))
            {
                for (const auto& file : _recent_files)
                {
                    if (ImGui::MenuItem(file.c_str()))
                    {
                        on_file_open(file);
                    }
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Switch Level", !_file_switcher.empty()))
            {
                for (const auto& file : _file_switcher)
                {
                    if (ImGui::MenuItem(file.path.c_str()))
                    {
                        on_file_open(file.path);
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
    }

    void ImGuiFileMenu::set_recent_files(const std::list<std::string>& files)
    {
        _recent_files = files | std::ranges::to<std::vector>();
    }

    void ImGuiFileMenu::switch_to(const std::string& filename)
    {
        filename;
    }

    void ImGuiFileMenu::set_sorting_mode(LevelSortingMode mode)
    {
        _sorting_mode = mode;
        sort_level_switcher();
    }

    void ImGuiFileMenu::sort_level_switcher()
    {
        switch (_sorting_mode)
        {
            case LevelSortingMode::FilenameOnly:
            {
                std::ranges::sort(_file_switcher, [](auto&& l, auto&& r) { return l.friendly_name < r.friendly_name; });
                break;
            }
            case LevelSortingMode::NameThenFilename:
            {
                // std::ranges::sort(_file_switcher, [](auto&& l, auto&& r) { return l.level_name < r.level_name; });
                // break;
            }
        }
    }

    void ImGuiFileMenu::receive_message(const Message& message)
    {
        if (auto settings = messages::read_settings(message))
        {
            set_sorting_mode(settings->level_sorting_mode);
            set_recent_files(settings->recent_files);
        }
    }
}
