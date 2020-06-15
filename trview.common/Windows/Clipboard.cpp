#include "Clipboard.h"
#include "../Window.h"

namespace trview
{
    std::wstring read_clipboard(const Window& window)
    {
        OpenClipboard(window);
        HANDLE data = GetClipboardData(CF_UNICODETEXT);
        std::wstring text(static_cast<wchar_t*>(data));
        CloseClipboard();
        return text;
    }
}
