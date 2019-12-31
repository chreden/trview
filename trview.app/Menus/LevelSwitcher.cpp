#include "LevelSwitcher.h"
#include <trview.app/Windows/WindowIDs.h>
#include "DirectoryListing.h"
#include <trview.common/Strings.h>

namespace trview
{
    namespace
    {
        const int ID_SWITCHFILE_BASE = 10000;

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
            SetMenuItemInfo(menu, ID_APP_FILE_SWITCHLEVEL, FALSE, &info);
            EnableMenuItem(menu, ID_APP_FILE_SWITCHLEVEL, MF_GRAYED);

            SetMenu(window, menu);

            DrawMenuBar(window);
            return directory_listing_menu;
        }
    }

    LevelSwitcher::LevelSwitcher(const Window& window)
        : MessageHandler(window), _directory_listing_menu(create_directory_listing_menu(window))
    {
    }

    void LevelSwitcher::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND)
        {
            int wmId = LOWORD(wParam);
            if (wmId >= ID_SWITCHFILE_BASE && wmId <= (ID_SWITCHFILE_BASE + GetMenuItemCount(_directory_listing_menu)))
            {
                const trview::File& f = _file_switcher_list.at(wmId - ID_SWITCHFILE_BASE);
                on_switch_level(f.path);
            }
        }
    }

    void LevelSwitcher::open_file(const std::string& filepath)
    {
        const std::size_t pos = filepath.find_last_of("\\/");
        const std::string folder = filepath.substr(0, pos);

        DirectoryListing dir_lister(folder);
        _file_switcher_list = dir_lister.GetFiles();

        // Enable menu when populating in case it's not enabled
        EnableMenuItem(GetMenu(window()), ID_APP_FILE_SWITCHLEVEL, MF_ENABLED);

        // Clear all items from menu and repopulate
        reset_menu(window(), _directory_listing_menu);
        for (auto i = 0u; i < _file_switcher_list.size(); ++i)
        {
            AppendMenu(_directory_listing_menu, MF_STRING, ID_SWITCHFILE_BASE + static_cast<int>(i), to_utf16(_file_switcher_list[i].friendly_name).c_str());
        }

        DrawMenuBar(window());
    }
}
