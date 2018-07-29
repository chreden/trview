#include "WindowResizer.h"
#include <trview.common/Window.h>

namespace trview
{
    WindowResizer::WindowResizer(HWND window)
        : MessageHandler(window), _previous_size(Window(window).size())
    {
    }

    void WindowResizer::process_message(HWND, UINT message, WPARAM wParam, LPARAM)
    {
        switch (message)
        {
            case WM_ENTERSIZEMOVE:
            {
                _resizing = true;
                break;
            }
            case WM_SIZE:
            {
                if (!_resizing && (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED) && has_size_changed())
                {
                    on_resize();
                }
                break;
            }
            case WM_EXITSIZEMOVE:
            {
                _resizing = false;
                if (has_size_changed())
                {
                    on_resize();
                }
                break;
            }
        }
    }

    bool WindowResizer::has_size_changed()
    {
        Size new_size = Window(window()).size();
        if (new_size.width == _previous_size.width && new_size.height == _previous_size.height)
        {
            return false;
        }
        _previous_size = new_size;
        return true;
    }
}
