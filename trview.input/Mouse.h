#pragma once

#include <windows.h>
#include <trview.common/Event.h>

namespace trview
{
    namespace input
    {
        class Mouse
        {
        public:
            enum class Button
            {
                Left,
                Right
            };

            Mouse();
            void process_input(const RAWINPUT& input);
            void process_scroll(int16_t delta);

            Event<Button> mouse_down;
            Event<Button> mouse_up;
            Event<int16_t> mouse_wheel;
            Event<long, long> mouse_move;

            long x() const;
            long y() const;
        private:
            void raise_absolute_mouse_move(long x, long y);

            bool _any_absolute_previous{ false };
            long _absolute_x;
            long _absolute_y;
        };
    }
}