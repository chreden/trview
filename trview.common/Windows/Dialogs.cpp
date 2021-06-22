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
                case IDialogs::Buttons::Yes_No:
                    return MB_YESNO;
                case IDialogs::Buttons::OK:
                default:
                    return MB_OK;
            }
        }

        bool convert_response(int response)
        {
            return response == IDOK || response == IDYES;
        }
    }

    bool Dialogs::message_box(const Window& window, const std::wstring& message, const std::wstring& title, Buttons buttons)
    {
        return convert_response(MessageBox(window, message.c_str(), title.c_str(), convert_button(buttons)));
    }
}
