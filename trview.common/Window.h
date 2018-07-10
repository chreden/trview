#pragma once

#include <Windows.h>
#include <cstdint>

#include "Point.h"

namespace trview
{
    class Window
    {
    public:
        Window(HWND window);
        uint32_t width() const;
        uint32_t height() const;
        HWND window() const;
        operator HWND () const;
    private:
        HWND        _window;
        uint32_t    _width;
        uint32_t    _height;
    };

    // Get the position of the cursor in client coordinates.
    // window: The client window.
    // Returns: The point in client coordinates.
    Point client_cursor_position(const Window& window) noexcept;

    // Determines whether the cursor is outside the bounds of the window.
    // window: The window to test the cursor against.
    // Returns: True if the cursor is outside the bounds of the window.
    bool cursor_outside_window(const Window& window) noexcept;

    // Determines whether the window is minimsed.
    // window: The window to test.
    // Returns: True if the window is minimised.
    bool window_is_minimised(const Window& window) noexcept;
}

