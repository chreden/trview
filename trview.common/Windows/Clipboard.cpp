#include "Clipboard.h"

namespace trview
{
    Clipboard::Clipboard(const Window& window)
        : _window(window)
    {
    }

    std::wstring Clipboard::read() const
    {
        OpenClipboard(_window);
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

    void Clipboard::write(const std::wstring& text)
    {
        const size_t length = text.size() * sizeof(wchar_t) + sizeof(wchar_t);
        HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, length);
        memcpy(GlobalLock(memory), text.c_str(), text.size() * sizeof(wchar_t));
        GlobalUnlock(memory);

        OpenClipboard(_window);
        EmptyClipboard();
        SetClipboardData(CF_UNICODETEXT, memory);
        CloseClipboard();
    }
}
