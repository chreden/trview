#include "Dialogs.h"

namespace trview
{
    bool Dialogs::message_box(const Window& window, const std::wstring& title, const std::wstring& message, Buttons buttons)
    {
        const auto mode = buttons == Buttons::OK_Cancel ? MB_OKCANCEL : MB_OK;
        return IDOK == MessageBox(window, title.c_str(), message.c_str(), mode);
    }
}
