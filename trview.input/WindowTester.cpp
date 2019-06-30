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
    }
}
