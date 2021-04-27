#pragma once

#include "../Window.h"
#include <string>

namespace trview
{
    struct IClipboard
    {
        virtual ~IClipboard() = 0;
        virtual std::wstring read(const Window& window) const = 0;
        virtual void write(const Window& window, const std::wstring& text) = 0;
    };
}
