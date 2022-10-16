#pragma once

#include "../Window.h"
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
