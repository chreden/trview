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
}

