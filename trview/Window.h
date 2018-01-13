#pragma once

#include <Windows.h>
#include <cstdint>

namespace trview
{
    class Window
    {
    public:
        Window(HWND window);
        uint32_t width() const;
        uint32_t height() const;
        HWND window() const;
    private:
        HWND        _window;
        uint32_t    _width;
        uint32_t    _height;
    };
}