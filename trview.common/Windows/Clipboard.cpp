#include "Clipboard.h"
#include "../Window.h"

namespace trview
{
    std::wstring read_clipboard(const Window& window)
    {
        OpenClipboard(window);
        HANDLE data = GetClipboardData(CF_UNICODETEXT);
        CloseClipboard();
        if (!data)
        {
            return std::wstring();
        }
        return std::wstring(static_cast<wchar_t*>(data));
    }
}
