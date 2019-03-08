#pragma once

#include <trview.common/Window.h>
#include <trview.common/TokenStore.h>
#include <trview.input/Mouse.h>

namespace trview
{
    namespace ui
    {
        class Control;

        /// Manages the mouse and keyboard input state for a control tree.
        class Input final
        {
        public:
            explicit Input(const trview::Window& window, Control& control);
        private:
            void     process_mouse_move();

            /// Find the control at the cursor position that has hover enabled.
            Control* hover_control_at_position(const Point& position);
            Control* hover_control_at_position(Control* control, const Point& position);
            bool     process_mouse_move(Control* control, const Point& position);


            TokenStore     _token_store;
            input::Mouse   _mouse;
            trview::Window _window;

            Control&       _control;
            Control*       _hover_control{ nullptr };
            Control*       _focus_control{ nullptr };
        };
    }
}