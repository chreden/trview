#include "ViewMenu.h"
#include "WindowIDs.h"

namespace trview
{
    namespace
    {
        bool is_checked(HMENU menu, UINT id)
        {
            MENUITEMINFO info;
            memset(&info, 0, sizeof(info));
            info.cbSize = sizeof(info);
            info.fMask = MIIM_STATE;
            GetMenuItemInfo(menu, id, FALSE, &info);
            return info.fState & MFS_CHECKED;
        }

        void set_checked(HMENU menu, UINT id, bool checked)
        {
            MENUITEMINFO info;
            memset(&info, 0, sizeof(info));
            info.cbSize = sizeof(info);
            info.fMask = MIIM_STATE;
            info.fState = (checked ? MFS_CHECKED : MFS_UNCHECKED);
            SetMenuItemInfo(menu, id, FALSE, &info);
        }
    }

    ViewMenu::ViewMenu(HWND window)
        : MessageHandler(window)
    {
        // Set all of the items to be initially visible.
        HMENU menu = GetMenu(window);
        MENUITEMINFO info;
        memset(&info, 0, sizeof(info));
        info.cbSize = sizeof(info);
        info.fMask = MIIM_STATE;
        info.fState = MFS_CHECKED;
        SetMenuItemInfo(menu, ID_APP_VIEW_MINIMAP, FALSE, &info);
        SetMenuItemInfo(menu, ID_APP_VIEW_TOOLTIP, FALSE, &info);
        SetMenuItemInfo(menu, ID_APP_VIEW_UI, FALSE, &info);
    }

    void ViewMenu::process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message != WM_COMMAND)
        {
            return;
        }

        HMENU menu = GetMenu(window);
        UINT id = LOWORD(wParam);
        bool checked = is_checked(menu, id);

        switch (id)
        {
            case ID_APP_VIEW_MINIMAP:
            {
                on_show_minimap(!checked);
                break;
            }
            case ID_APP_VIEW_TOOLTIP:
            {
                on_show_tooltip(!checked);
                break;
            }
            case ID_APP_VIEW_UI:
            {
                on_show_ui(!checked);
                break;
            }
            default:
            {
                return;
            }
        }

        set_checked(menu, id, !checked);
    }
}
