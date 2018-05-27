#include "Keyboard.h"
#include <Windows.h>
#include <algorithm>

namespace trview
{
    namespace input
    {
        bool Keyboard::control() const
        {
            return GetAsyncKeyState(VK_CONTROL);
        }
    }
}
