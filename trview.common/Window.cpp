#include "stdafx.h"
#include "Window.h"
#include "Strings.h"

namespace trview
{
    Window::Window(HWND window)
        : _window(window)
    {
    }

    Size Window::size() const
    {
        RECT rectangle;
        GetClientRect(_window, &rectangle);
        return Size(static_cast<float>(rectangle.right - rectangle.left), static_cast<float>(rectangle.bottom - rectangle.top));
    }

    HWND Window::window() const
    {
        return _window;
    }

    Window::operator HWND () const
    {
        return _window;
    }

    void Window::set_title(const std::string& title)
    {
        auto converted = to_utf16(title);
        SetWindowText(_window, converted.c_str());
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
        const Size size = window.size();
        return cursor_pos.x < 0 || cursor_pos.y < 0 || cursor_pos.x > size.width || cursor_pos.y > size.height;
    }

    // Determines whether the window is minimsed.
    // window: The window to test.
    // Returns: True if the window is minimised.
    bool window_is_minimised(const Window& window) noexcept
    {
        return IsIconic(window.window());
    }

    Window window_under_cursor() noexcept
    {
        POINT cursor_pos;
        GetCursorPos(&cursor_pos);
        return WindowFromPoint(cursor_pos);
    }
}
