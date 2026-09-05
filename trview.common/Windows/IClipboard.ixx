export module trview.common:IClipboard;

import std;
import :Window;

namespace trview
{
    export struct IClipboard
    {
        virtual ~IClipboard() = 0;
        virtual std::wstring read() const = 0;
        virtual void write(const std::wstring& text) = 0;
    };
}
