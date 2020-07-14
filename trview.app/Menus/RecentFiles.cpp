#include "RecentFiles.h"
#include <trview.app/Windows/WindowIDs.h>
#include <trview.common/Strings.h>

namespace trview
{
    namespace
    {
        const int ID_RECENT_FILE_BASE = 5000;

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
            SetMenuItemInfo(menu, ID_APP_FILE_OPENRECENT, FALSE, &info);
            SetMenu(window, menu);
        }
    }

    RecentFiles::RecentFiles(const Window& window)
        : MessageHandler(window)
    {
    }

    void RecentFiles::process_message(UINT message, WPARAM wParam, LPARAM)
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
        }
    }

    void RecentFiles::set_recent_files(const std::list<std::string>& files)
    {
        _recent_files.assign(files.begin(), files.end());
        update_menu(window(), _recent_files);
    }
}
