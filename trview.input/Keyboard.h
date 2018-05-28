#pragma once

#include <cstdint>
#include <vector>
#include <functional>
#include <trview.common/Event.h>
#include <Windows.h>

namespace trview
{
    namespace input
    {
        class Keyboard
        {
        public:
            explicit Keyboard(HWND window);

            bool control() const;
            Event<uint16_t> on_key_down;
            Event<uint16_t> on_key_up;
            Event<uint16_t> on_char;
        private:
            HWND _window;
        };
    }
}
