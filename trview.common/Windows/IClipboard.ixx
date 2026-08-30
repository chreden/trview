export module trview.common:IClipboard;

import :Window;
#include <string>

namespace trview
{
    struct IClipboard
    {
        virtual ~IClipboard() = 0;
        virtual std::wstring read() const = 0;
        virtual void write(const std::wstring& text) = 0;
    };
}
