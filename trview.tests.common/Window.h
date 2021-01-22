#pragma once

#include <string>
#include <trview.common/Window.h>

namespace trview
{
    namespace tests
    {
        /// Makes a hidden test window that can be used to pass messages to for tests.
        /// @param name The name to use for the window class and window name.
        /// @param parent The parent window - defaults to HWND_MESSAGE.
        Window create_test_window(const std::wstring& name, HWND parent = HWND_MESSAGE);
    }
}