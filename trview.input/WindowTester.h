#pragma once

#include "IWindowTester.h"

namespace trview
{
    namespace input
    {
        class WindowTester final : public IWindowTester
        {
        public:
            virtual ~WindowTester() = default;
            virtual Window window_under_cursor() const override;
            virtual int screen_width(bool rdp) const override;
            virtual int screen_height(bool rdp) const override;
        };
    }
}
