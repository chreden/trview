#include "stdafx.h"
#include "WindowResizer.h"

namespace trview
{
    WindowResizer::WindowResizer(HWND window)
        : MessageHandler(window)
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
                if (!_resizing && (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED))
                {
                    on_resize();
                }
                break;
            }
            case WM_EXITSIZEMOVE:
            {
                _resizing = false;
                on_resize();
                break;
            }
        }
    }
}
