#include "ImGuiFileMenu.h"
#include <trview.common/Strings.h>
#include "../Messages/Messages.h"
#include "../Settings/UserSettings.h"

namespace trview
{
    ImGuiFileMenu::ImGuiFileMenu(const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IFiles>& files, const LevelNameSource& level_name_source)
        : _dialogs(dialogs), _files(files), _level_name_source(level_name_source)
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
            _file_switcher = valid_pack_levels(*pack_ptr)
                | std::views::transform([&](auto&& f) -> File { return { .path = f.path, .friendly_name = f.friendly_name, .level_name = _level_name_source(f.path, pack_ptr) }; })
                | std::ranges::to<std::vector>();
        }
        else
        {
            _file_switcher = _files->get_files(path_for_filename(filename), default_file_pattern)
                | std::views::transform([&](auto&& f) -> File { return { .path = f.path, .friendly_name = f.friendly_name, .level_name = _level_name_source(f.path, {}) }; })
                | std::ranges::to<std::vector>();
        }
        sort_level_switcher();
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
                    const auto name = file.level_name.has_value() ? std::format("{} ({})", file.level_name.value().name, file.friendly_name) : file.friendly_name;
                    if (ImGui::MenuItem(name.c_str()))
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
            case LevelSortingMode::Full:
            {
                std::ranges::sort(_file_switcher, [](auto&& l, auto&& r) { return
                    std::tuple(l.level_name.value_or({}).index, l.level_name.value_or({}).name) <
                    std::tuple(r.level_name.value_or({}).index, r.level_name.value_or({}).name); });
                break;
            }
            case LevelSortingMode::FilenameOnly:
            {
                std::ranges::sort(_file_switcher, [](auto&& l, auto&& r) { return l.friendly_name < r.friendly_name; });
                break;
            }
            case LevelSortingMode::NameThenFilename:
            {
                std::ranges::sort(_file_switcher, [](auto&& l, auto&& r) { return l.level_name.value_or({}).name < r.level_name.value_or({}).name; });
                break;
            }
        }
    }

    void ImGuiFileMenu::receive_message(const Message& message)
    {
        if (auto settings = messages::read_settings(message))
        {
            set_sorting_mode(settings->level_sorting_mode);
            set_recent_files(settings->recent_diff_files);
        }
    }
}
