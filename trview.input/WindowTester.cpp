#include "WindowTester.h"
#include <trview.common/Window.h>

namespace trview
{
    namespace input
    {
        WindowTester::WindowTester(const Window& window)
            : MessageHandler(window)
        {
        }

        bool WindowTester::is_window_under_cursor() const
        {
            return _mouse_in_window;
        }

        int WindowTester::screen_width(bool rdp) const
        {
            return GetSystemMetrics(rdp ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);
        }

        int WindowTester::screen_height(bool rdp) const
        {
            return GetSystemMetrics(rdp ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);
        }

        void WindowTester::process_message(UINT message, WPARAM, LPARAM)
        {
            if (message == WM_MOUSEMOVE && !_mouse_in_window)
            {
                _mouse_in_window = true;
                TRACKMOUSEEVENT track = { sizeof(track) };
                track.dwFlags = TME_LEAVE;
                track.hwndTrack = window();
                TrackMouseEvent(&track);
            }
            else if (message == WM_MOUSELEAVE)
            {
                _mouse_in_window = false;
            }
        }
    }
}
