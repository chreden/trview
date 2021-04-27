#include "Clipboard.h"
#include "../Window.h"

namespace trview
{
    std::wstring Clipboard::read(const Window& window) const
    {
        OpenClipboard(window);
        HANDLE data = GetClipboardData(CF_UNICODETEXT);
        if (!data)
        {
            return std::wstring();
        }

        auto ptr = static_cast<wchar_t*>(GlobalLock(data));
        auto result = std::wstring(ptr);
        GlobalUnlock(ptr);
        CloseClipboard();
        return result;
    }

    void Clipboard::write(const Window& window, const std::wstring& text)
    {
        const size_t length = text.size() * sizeof(wchar_t) + sizeof(wchar_t);
        HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, length);
        memcpy(GlobalLock(memory), text.c_str(), text.size() * sizeof(wchar_t));
        GlobalUnlock(memory);

        OpenClipboard(window);
        EmptyClipboard();
        SetClipboardData(CF_UNICODETEXT, memory);
        CloseClipboard();
    }
}
