#include "MenuDetector.h"

namespace trview
{
    MenuDetector::MenuDetector(const Window& window)
        : MessageHandler(window)
    {
    }

    std::optional<int> MenuDetector::process_message(UINT message, WPARAM, LPARAM)
    {
        if (message == WM_ENTERMENULOOP)
        {
            on_menu_toggled(true);
        }
        else if (message == WM_EXITMENULOOP)
        {
            on_menu_toggled(false);
        }
        return {};
    }
}
