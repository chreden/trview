#include "AlternateGroupToggler.h"

namespace trview
{
    AlternateGroupToggler::AlternateGroupToggler(const Window& window)
        : MessageHandler(window)
    {
    }

    void AlternateGroupToggler::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_CHAR)
        {
            const wchar_t value = static_cast<wchar_t>(wParam);
            if (value >= L'0' && value <= L'9')
            {
                on_alternate_group(value - L'0');
            }
        }
    }
}
