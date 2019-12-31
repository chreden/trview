#include "ViewMenu.h"
#include <trview.app/Windows/WindowIDs.h>

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
            GetMenuItemInfo(menu, id, FALSE, &info);
            
            info.fState ^= (checked ? MFS_UNCHECKED : MFS_CHECKED);
            info.fState |= (checked ? MFS_CHECKED : MFS_UNCHECKED);
            SetMenuItemInfo(menu, id, FALSE, &info);
        }

        void set_enabled(HMENU menu, UINT id, bool enabled)
        {
            MENUITEMINFO info;
            memset(&info, 0, sizeof(info));
            info.cbSize = sizeof(info);
            info.fMask = MIIM_STATE;
            GetMenuItemInfo(menu, id, FALSE, &info);

            info.fState ^= (enabled ? MFS_DISABLED : MFS_ENABLED);
            info.fState |= (enabled ? MFS_ENABLED : MFS_DISABLED);
            SetMenuItemInfo(menu, id, FALSE, &info);
        }
    }

    ViewMenu::ViewMenu(const Window& window)
        : MessageHandler(window)
    {
        // Set all of the items to be initially visible.
        HMENU menu = GetMenu(window);
        set_checked(menu, ID_APP_VIEW_MINIMAP, true);
        set_checked(menu, ID_APP_VIEW_TOOLTIP, true);
        set_checked(menu, ID_APP_VIEW_UI, true);
        set_checked(menu, ID_APP_VIEW_COMPASS, true);
        set_checked(menu, ID_APP_VIEW_SELECTION, true);
        set_checked(menu, ID_APP_VIEW_ROUTE, true);
        set_checked(menu, ID_APP_VIEW_TOOLS, true);
    }

    void ViewMenu::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message != WM_COMMAND)
        {
            return;
        }

        HMENU menu = GetMenu(window());
        UINT id = LOWORD(wParam);
        bool enable = !is_checked(menu, id);

        switch (id)
        {
            case ID_APP_VIEW_MINIMAP:
            {
                on_show_minimap(enable);
                break;
            }
            case ID_APP_VIEW_TOOLTIP:
            {
                on_show_tooltip(enable);
                break;
            }
            case ID_APP_VIEW_COMPASS:
            {
                on_show_compass(enable);
                break;
            }
            case ID_APP_VIEW_SELECTION:
            {
                on_show_selection(enable);
                break;
            }
            case ID_APP_VIEW_ROUTE:
            {
                on_show_route(enable);
                break;
            }
            case ID_APP_VIEW_TOOLS:
            {
                on_show_tools(enable);
                break;
            }
            case ID_APP_VIEW_UI:
            {
                // If the UI element is toggled, then enable/disable all other elements.
                set_enabled(menu, ID_APP_VIEW_MINIMAP, enable);
                set_enabled(menu, ID_APP_VIEW_TOOLTIP, enable);
                set_enabled(menu, ID_APP_VIEW_COMPASS, enable);
                set_enabled(menu, ID_APP_VIEW_SELECTION, enable);
                set_enabled(menu, ID_APP_VIEW_ROUTE, enable);
                set_enabled(menu, ID_APP_VIEW_TOOLS, enable);
                on_show_ui(enable);

                // Raise event to disable all other elements as they are sub-elements of the UI. However, do not alter
                // their checked state in the UI so that it is preserved for when the UI main element is re-enabled.
                if (!enable)
                {
                    on_show_minimap(false);
                    on_show_tooltip(false);
                    on_show_compass(false);
                    on_show_selection(false);
                    on_show_route(false);
                    on_show_tools(false);
                }
                else
                {
                    // Return the states of the settings to what they were before the main UI toggle was changed.
                    on_show_minimap(is_checked(menu, ID_APP_VIEW_MINIMAP));
                    on_show_tooltip(is_checked(menu, ID_APP_VIEW_TOOLTIP));
                    on_show_compass(is_checked(menu, ID_APP_VIEW_COMPASS));
                    on_show_selection(is_checked(menu, ID_APP_VIEW_SELECTION));
                    on_show_route(is_checked(menu, ID_APP_VIEW_ROUTE));
                    on_show_tools(is_checked(menu, ID_APP_VIEW_TOOLS));
                }
                break;
            }
            default:
            {
                return;
            }
        }

        set_checked(menu, id, enable);
    }
}
