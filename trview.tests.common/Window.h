#pragma once

#include <windows.h>
#include <string>

namespace trview
{
    namespace tests
    {
        /// Makes a hidden test window that can be used to pass messages to for tests.
        /// @param name The name to use for the window class and window name.
        HWND create_test_window(const std::wstring& name);
    }
}