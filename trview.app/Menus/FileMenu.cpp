#include "FileMenu.h"
#include <trview.common/Strings.h>
#include "../Messages/Messages.h"
#include "../Settings/UserSettings.h"
#include "../Elements/ILevel.h"

namespace trview
{
    namespace
    {
        std::string trimmed_level_name(const std::string& input)
        {
            return input.substr(input.find_last_of("/\\") + 1);
        }
    }

    IFileMenu::~IFileMenu()
    {
    }

    FileMenu::FileMenu(const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IFiles>& files, const LevelNameSource& level_name_source, Mode mode, const std::weak_ptr<IMessageSystem>& messaging)
        : _dialogs(dialogs), _files(files), _level_name_source(level_name_source), _mode(mode), _messaging(messaging)
    {
    }

    std::vector<std::string> FileMenu::local_levels() const
    {
        std::vector<std::string> files;
        for (const auto& f : _file_switcher)
        {
            files.push_back(f.path);
        }
        return files;
    }

    void FileMenu::open_file(const std::string& filename, const std::weak_ptr<trlevel::IPack>& pack)
    {
        _opened_file = filename;
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
        _reload_enabled = true;
        messages::send_local_levels(_messaging, local_levels());
    }

    void FileMenu::render()
    {
        if (ImGui::Shortcut(ImGuiMod_Alt | ImGuiKey_F, 0U, ImGuiInputFlags_RouteGlobal))
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            const ImGuiID id = window->GetID("File");
            ImGui::OpenPopup(id, ImGuiPopupFlags_None);
        }

        if (_mode == Mode::Main)
        {
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O, 0U, ImGuiInputFlags_RouteGlobal))
            {
                open();
            }

            if (ImGui::Shortcut(ImGuiKey_F5, 0U, ImGuiInputFlags_RouteGlobal))
            {
                on_reload();
            }

            if (ImGui::Shortcut(ImGuiKey_F6, 0U, ImGuiInputFlags_RouteGlobal))
            {
                previous_directory_file();
            }

            if (ImGui::Shortcut(ImGuiKey_F7, 0U, ImGuiInputFlags_RouteGlobal))
            {
                next_directory_file();
            }
        }

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open"))
            {
                open();
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
                        switch_to(file.path);
                    }
                }
                ImGui::EndMenu();
            }

            if (_mode == Mode::Main)
            {
                if (ImGui::MenuItem("Reload", "F5", nullptr, _reload_enabled))
                {
                    on_reload();
                }

                if (ImGui::MenuItem("Exit"))
                {
                    messages::commands::send_quit(_messaging);
                }
            }

            ImGui::EndMenu();
        }
    }

    void FileMenu::set_recent_files(const std::list<std::string>& files)
    {
        _recent_files = files | std::ranges::to<std::vector>();
    }

    void FileMenu::switch_to(const std::string& filename)
    {
        const auto found = std::ranges::find_if(
            _file_switcher,
            [&](const auto& x) -> bool
            {
                return trimmed_level_name(x.friendly_name) == trimmed_level_name(filename);
            });
        if (found != _file_switcher.end())
        {
            on_file_open(found->path);
        }
    }

    void FileMenu::set_sorting_mode(LevelSortingMode mode)
    {
        _sorting_mode = mode;
        sort_level_switcher();
    }

    void FileMenu::sort_level_switcher()
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

    void FileMenu::receive_message(const Message& message)
    {
        if (auto settings = messages::read_settings(message))
        {
            set_sorting_mode(settings->level_sorting_mode);
            set_recent_files(_mode == Mode::Main ? settings->recent_files : settings->recent_diff_files);
        }
        else if (auto level = messages::read_open_level(message))
        {
            if (auto level_ptr = level->lock())
            {
                open_file(level_ptr->filename(), level_ptr->pack());
            }
        }
        else if (auto switch_to_filename = messages::read_switch_level_filename(message))
        {
            switch_to(switch_to_filename.value());
        }
    }

    void FileMenu::open()
    {
        const auto filename = _dialogs->open_file(L"Open level", { { L"All Tomb Raider Files", { L"*.tr2", L"*.tr4", L"*.trc", L"*.phd", L"*.psx", L"*.obj", L"*.tom", L"*.sat" } } }, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, _initial_directory);
        if (filename.has_value())
        {
            _initial_directory = filename->directory;
            on_file_open(filename->filename);
        }
    }

    void FileMenu::previous_directory_file()
    {
        auto iter = std::find_if(_file_switcher.begin(), _file_switcher.end(),
            [&](const auto& f) { return f.path == _opened_file; });
        if (iter == _file_switcher.end() || iter == _file_switcher.begin())
        {
            return;
        }
        --iter;
        on_file_open(iter->path);
    }

    void FileMenu::next_directory_file()
    {
        auto iter = std::find_if(_file_switcher.begin(), _file_switcher.end(),
            [&](const auto& f) { return f.path == _opened_file; });
        if (iter == _file_switcher.end())
        {
            return;
        }
        if (++iter != _file_switcher.end())
        {
            on_file_open(iter->path);
        }
    }
}
