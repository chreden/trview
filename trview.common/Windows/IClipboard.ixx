export module trview.common:IClipboard;

#include <string>

namespace trview
{
    export struct IClipboard
    {
        virtual ~IClipboard() = default;
        virtual std::wstring read() const = 0;
        virtual void write(const std::wstring& text) = 0;
    };
}
