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

            Event<Button> mouse_down;
            Event<Button> mouse_up;
            Event<int16_t> mouse_wheel;
            Event<long, long> mouse_move;

        private:
            void raise_mouse_down(Button button);
            void raise_mouse_up(Button button);
            void raise_mouse_wheel(short delta);
            void raise_relative_mouse_move(long x, long y);
            void raise_absolute_mouse_move(long x, long y);

            bool _any_absolute_previous{ false };
            long _absolute_x;
            long _absolute_y;
        };
    }
}