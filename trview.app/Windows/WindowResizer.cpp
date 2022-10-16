#include "WindowResizer.h"
#include <trview.common/Window.h>

namespace trview
{
    const int32_t WindowResizer::WM_APP_PARENT_RESTORED = WM_APP + 1;

    WindowResizer::WindowResizer(const Window& window)
        : MessageHandler(window), _previous_size(Window(window).size())
    {
    }

    std::optional<int> WindowResizer::process_message(UINT message, WPARAM wParam, LPARAM)
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
                if (!_resizing && (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED || has_size_changed()))
                {
                    on_resize();

                    // Inform owned windows that the window has been restored.
                    EnumThreadWindows(GetCurrentThreadId(),
                        [](auto window, auto) -> BOOL 
                        {
                            SendMessage(window, WM_APP_PARENT_RESTORED, 0, 0);
                            return true;
                        }, 0);
                }
                break;
            }
            case WM_EXITSIZEMOVE:
            {
                _resizing = false;
                on_resize();
                break;
            }
            case WM_APP_PARENT_RESTORED:
            {
                on_resize();
                break;
            }
        }
        return {};
    }

    bool WindowResizer::has_size_changed()
    {
        Size new_size = Window(window()).size();
        if (new_size == _previous_size)
        {
            return false;
        }
        _previous_size = new_size;
        return true;
    }
}
