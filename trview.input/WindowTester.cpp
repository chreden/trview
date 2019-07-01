#include "WindowTester.h"
#include <trview.common/Window.h>

namespace trview
{
    namespace input
    {
        Window WindowTester::window_under_cursor() const
        {
            return trview::window_under_cursor();
        }

        int WindowTester::screen_width(bool rdp) const
        {
            return GetSystemMetrics(rdp ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);
        }

        int WindowTester::screen_height(bool rdp) const
        {
            return GetSystemMetrics(rdp ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);
        }
    }
}
