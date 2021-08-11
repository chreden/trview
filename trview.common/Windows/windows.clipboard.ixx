module;
#include <string>
#include <windows.h>
export module trview.common.windows.clipboard;

import trview.common.windows.window;

namespace trview
{
    export struct IClipboard
    {
        virtual ~IClipboard() = 0;
        virtual std::wstring read(const Window& window) const = 0;
        virtual void write(const Window& window, const std::wstring& text) = 0;
    };

    export class Clipboard final : public IClipboard
    {
    public:
        virtual ~Clipboard() = default;

        virtual std::wstring read(const Window& window) const override
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

        virtual void write(const Window& window, const std::wstring& text) override
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
    };
}