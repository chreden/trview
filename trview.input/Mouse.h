#pragma once

#include <windows.h>
#include <trview.common/Event.h>
#include <cstdint>
#include <memory>
#include <unordered_map>

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

            explicit Mouse(HWND window);
            ~Mouse();
            void process_input(const RAWINPUT& input);
            void process_scroll(int16_t delta);

            Event<Button> mouse_down;
            Event<Button> mouse_up;
            Event<int16_t> mouse_wheel;
            Event<long, long> mouse_move;

            long x() const;
            long y() const;

            /// Type definition for the static all mice map. The mouse should refer to
            /// the member variable to access the mouse map as the static instance may have
            /// been destroyed at the time that the mouse is destroyed.
            using MouseMap = std::unordered_map<HWND, std::vector<Mouse*>>;
        private:
            void raise_absolute_mouse_move(long x, long y);

            bool _any_absolute_previous{ false };
            long _absolute_x;
            long _absolute_y;
            HWND _window;

            std::shared_ptr<MouseMap> _all_mice;
        };
    }
}