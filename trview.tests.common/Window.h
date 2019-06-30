#pragma once

#include <string>
#include <trview.common/Window.h>

namespace trview
{
    namespace tests
    {
        /// Makes a hidden test window that can be used to pass messages to for tests.
        /// @param name The name to use for the window class and window name.
        Window create_test_window(const std::wstring& name);
    }
}