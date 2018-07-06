#include "RecentFiles.h"
#include "WindowIDs.h"

namespace trview
{
    namespace
    {
        const int ID_RECENT_FILE_BASE = 5000;

        void update_menu(HWND window, const std::vector<std::wstring>& recent_files)
        {
            // Set up the recently used files menu.
            HMENU menu = GetMenu(window);
            HMENU popup = CreatePopupMenu();

            for (int i = 0; i < recent_files.size(); ++i)
            {
                AppendMenu(popup, MF_STRING, ID_RECENT_FILE_BASE + i, recent_files[i].c_str());
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

    RecentFiles::RecentFiles(HWND window)
        : MessageHandler(window)
    {
    }

    void RecentFiles::process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_COMMAND)
        {
            int wmId = LOWORD(wParam);
            if (wmId >= ID_RECENT_FILE_BASE && wmId <= ID_RECENT_FILE_BASE + _recent_files.size())
            {
                int index = wmId - ID_RECENT_FILE_BASE;
                if (index >= 0 && index < _recent_files.size())
                {
                    on_file_open(_recent_files[index]);
                }
            }
        }
    }

    void RecentFiles::set_recent_files(const std::list<std::wstring>& files)
    {
        _recent_files.assign(files.begin(), files.end());
        update_menu(window(), _recent_files);
    }
}
