#pragma once

#include <trview.common/Window.h>

namespace trview
{
    namespace input
    {
        struct IWindowTester
        {
            virtual ~IWindowTester() = 0;
            virtual Window window_under_cursor() const = 0;
            virtual int screen_width(bool rdp) const = 0;
            virtual int screen_height(bool rdp) const = 0;
        };
    }
}
