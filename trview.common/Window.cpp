#include "stdafx.h"
#include "Window.h"

namespace trview
{
    Window::Window(HWND window)
        : _window(window)
    {
        RECT rectangle;
        GetClientRect(_window, &rectangle);
        _width = rectangle.right - rectangle.left;
        _height = rectangle.bottom - rectangle.top;
    }

    uint32_t Window::width() const
    {
        return _width;
    }

    uint32_t Window::height() const
    {
        return _height;
    }

    HWND Window::window() const
    {
        return _window;
    }

    // Get the position of the cursor in client coordinates.
    // window: The client window.
    // Returns: The point in client coordinates.
    Point client_cursor_position(const Window& window) noexcept
    {
        POINT cursor_pos;
        GetCursorPos(&cursor_pos);
        ScreenToClient(window.window(), &cursor_pos);
        return Point(static_cast<float>(cursor_pos.x), static_cast<float>(cursor_pos.y));
    }

    // Determines whether the cursor is outside the bounds of the window.
    // window: The window to test the cursor against.
    // Returns: True if the cursor is outside the bounds of the window.
    bool cursor_outside_window(const Window& window) noexcept
    {
        const Point cursor_pos = client_cursor_position(window);
        return cursor_pos.x < 0 || cursor_pos.y < 0 || cursor_pos.x > window.width() || cursor_pos.y > window.height();
    }

    // Determines whether the window is minimsed.
    // window: The window to test.
    // Returns: True if the window is minimised.
    bool window_is_minimised(const Window& window) noexcept
    {
        return IsIconic(window.window());
    }
}
