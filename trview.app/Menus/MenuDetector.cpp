#include "MenuDetector.h"

namespace trview
{
    MenuDetector::MenuDetector(const Window& window)
        : MessageHandler(window)
    {
    }

    void MenuDetector::process_message(UINT message, WPARAM wParam, LPARAM lParam) 
    {
        if (message == WM_MENUSELECT)
        {
            bool closing = HIWORD(wParam) == 0xffff;
            on_menu_toggled(!closing);
        }
    }
}
