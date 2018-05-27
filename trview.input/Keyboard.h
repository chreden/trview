#pragma once

#include <cstdint>
#include <vector>
#include <functional>
#include <trview.common/Event.h>

namespace trview
{
    namespace input
    {
        class Keyboard
        {
        public:
            bool control() const;
            Event<uint16_t> on_key_down;
            Event<uint16_t> on_key_up;
            Event<uint16_t> on_char;
        };
    }
}
