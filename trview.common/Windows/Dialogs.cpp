#include "Dialogs.h"

namespace trview
{
    namespace
    {
        long convert_button(IDialogs::Buttons buttons)
        {
            switch (buttons)
            {
                case IDialogs::Buttons::OK_Cancel:
                    return MB_OKCANCEL;
                case IDialogs::Buttons::OK:
                default:
                    return MB_OK;
            }
        }
    }

    bool Dialogs::message_box(const Window& window, const std::wstring& message, const std::wstring& title, Buttons buttons)
    {
        return IDOK == MessageBox(window, message.c_str(), title.c_str(), convert_button(buttons));
    }
}
