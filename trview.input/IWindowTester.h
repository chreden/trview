#pragma once

#include <trview.common/Window.h>

namespace trview
{
    namespace input
    {
        /// Used to check for Windows information.
        struct IWindowTester
        {
            virtual ~IWindowTester() = 0;

            /// Gets the window that is currently under the cursor.
            virtual Window window_under_cursor() const = 0;

            /// Gets the screen width - or the virtual screen width if on remote desktop.
            /// @param rdp Whether to check the remote screen size.
            virtual int screen_width(bool rdp) const = 0;

            /// Gets the screen height - or the virtual screen height if on remote desktop.
            /// @param rdp Whether to check the remote screen size.
            virtual int screen_height(bool rdp) const = 0;
        };
    }
}
