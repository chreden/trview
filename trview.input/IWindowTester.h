#pragma once

#include <memory>

#include <trview.common/Window.h>

namespace trview
{
    namespace input
    {
        /// Used to check for Windows information.
        struct IWindowTester
        {
            using Source = std::function<std::unique_ptr<IWindowTester>(const Window&)>;

            virtual ~IWindowTester() = 0;

            /// Is the window currently under the cursor?
            virtual bool is_window_under_cursor() const = 0;

            /// Gets the screen width - or the virtual screen width if on remote desktop.
            /// @param rdp Whether to check the remote screen size.
            virtual int screen_width(bool rdp) const = 0;

            /// Gets the screen height - or the virtual screen height if on remote desktop.
            /// @param rdp Whether to check the remote screen size.
            virtual int screen_height(bool rdp) const = 0;
        };
    }
}
