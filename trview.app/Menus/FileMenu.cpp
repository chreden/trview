#include "FileMenu.h"
#include "../Resources/resource.h"

namespace trview
{
    namespace
    {
        constexpr int ID_RECENT_FILE_BASE = 5000;
        constexpr int ID_SWITCHFILE_BASE = 10000;

        void update_menu(const Window& window, const std::vector<std::string>& recent_files)
        {
            // Set up the recently used files menu.
            HMENU menu = GetMenu(window);
            HMENU popup = CreatePopupMenu();

            for (auto i = 0u; i < recent_files.size(); ++i)
            {
                AppendMenu(popup, MF_STRING, ID_RECENT_FILE_BASE + i, to_utf16(recent_files[i]).c_str());
            }

            MENUITEMINFO info;
            memset(&info, 0, sizeof(info));
            info.cbSize = sizeof(info);
            info.fMask = MIIM_SUBMENU;
            info.hSubMenu = popup;
            SetMenuItemInfo(menu, ID_FILE_OPENRECENT, FALSE, &info);
            SetMenu(window, menu);
        }

        /// Reset the menu to have no items in it.
        /// @param window The window that the menu bar belongs to.
        /// @param menu The menu to clear.
        void reset_menu(const Window& window, HMENU menu)
        {
            int count = GetMenuItemCount(menu);

            while ((count = GetMenuItemCount(menu)) > 0)
                RemoveMenu(menu, 0, MF_BYPOSITION);

            DrawMenuBar(window);
        }

        /// Create the directory listing menu as a child of the menu in the specified window.
        /// @param window The window that owns the menu.
        /// @returns The new menu.
        HMENU create_directory_listing_menu(const Window& window)
        {
            HMENU menu = GetMenu(window);
            HMENU directory_listing_menu = CreatePopupMenu();

            MENUITEMINFO info;
            memset(&info, 0, sizeof(info));

            info.cbSize = sizeof(info);
            info.fMask = MIIM_SUBMENU;
            info.hSubMenu = directory_listing_menu;

            // Set up the popup menu and grey it out initially - only when it's populated do we enable it
            SetMenuItemInfo(menu, ID_FILE_SWITCHLEVEL, FALSE, &info);
            EnableMenuItem(menu, ID_FILE_SWITCHLEVEL, MF_GRAYED);

            SetMenu(window, menu);

            DrawMenuBar(window);
            return directory_listing_menu;
        }
    }

    IFileMenu::~IFileMenu()
    {
    }

    FileMenu::FileMenu(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const std::shared_ptr<IDialogs>& dialogs)
        : MessageHandler(window), _dialogs(dialogs), _directory_listing_menu(create_directory_listing_menu(window))
    {
        DragAcceptFiles(window, TRUE);

        HMENU menu = GetMenu(window);
        EnableMenuItem(menu, ID_FILE_RELOAD, MF_GRAYED);

        _token_store += shortcuts->add_shortcut(false, VK_F5) += [&]() { on_reload(); };
        _token_store += shortcuts->add_shortcut(true, 'O') += [&]() { choose_file(); };
    }

    std::optional<int> FileMenu::process_message(UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_COMMAND)
        {
            int wmId = LOWORD(wParam);
            if (wmId >= ID_RECENT_FILE_BASE && wmId <= ID_RECENT_FILE_BASE + static_cast<int>(_recent_files.size()))
            {
                int index = wmId - ID_RECENT_FILE_BASE;
                if (index >= 0 && index < static_cast<int>(_recent_files.size()))
                {
                    on_file_open(_recent_files[index]);
                }
            }
            else if (wmId >= ID_SWITCHFILE_BASE && wmId <= (ID_SWITCHFILE_BASE + GetMenuItemCount(_directory_listing_menu)))
            {
                const trview::File& f = _file_switcher_list.at(wmId - ID_SWITCHFILE_BASE);
                on_file_open(f.path);
            }
            else if (wmId == ID_FILE_RELOAD)
            {
                on_reload();
            }
            else if (wmId == ID_FILE_OPEN)
            {
                choose_file();
            }
        }
        else if (message == WM_DROPFILES)
        {
            wchar_t filename[MAX_PATH];
            memset(&filename, 0, sizeof(filename));
            DragQueryFile((HDROP)wParam, 0, filename, MAX_PATH);
            on_file_open(to_utf8(filename));
        }

        return std::nullopt;
    }

    void FileMenu::open_file(const std::string& filename)
    {
        const std::size_t pos = filename.find_last_of("\\/");
        const std::string folder = filename.substr(0, pos);

        DirectoryListing dir_lister(folder);
        _file_switcher_list = dir_lister.GetFiles();

        // Enable menu when populating in case it's not enabled
        EnableMenuItem(GetMenu(window()), ID_FILE_SWITCHLEVEL, MF_ENABLED);

        // Clear all items from menu and repopulate
        reset_menu(window(), _directory_listing_menu);
        for (auto i = 0u; i < _file_switcher_list.size(); ++i)
        {
            AppendMenu(_directory_listing_menu, MF_STRING, ID_SWITCHFILE_BASE + static_cast<int>(i), to_utf16(_file_switcher_list[i].friendly_name).c_str());
        }

        EnableMenuItem(GetMenu(window()), ID_FILE_RELOAD, MF_ENABLED);
        DrawMenuBar(window());
    }

    void FileMenu::set_recent_files(const std::list<std::string>& files)
    {
        _recent_files.assign(files.begin(), files.end());
        update_menu(window(), _recent_files);
    }

    void FileMenu::choose_file()
    {
        const auto filename = _dialogs->open_file(L"Open level", { { L"All Tomb Raider Files", { L"*.tr*", L"*.phd" } } }, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST);
        if (filename.has_value())
        {
            on_file_open(filename.value().filename);
        }
    }
}