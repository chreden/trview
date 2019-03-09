#pragma once

#include <trview.common/Window.h>
#include <trview.common/TokenStore.h>
#include <trview.input/Mouse.h>
#include <trview.input/Keyboard.h>
#include "IInputQuery.h"

namespace trview
{
    namespace ui
    {
        class Control;

        /// Manages the mouse and keyboard input state for a control tree.
        class Input final : public IInputQuery
        {
        public:
            explicit Input(const trview::Window& window, Control& control);
            virtual ~Input() = default;
            virtual Control* focus_control() const;
        private:
            void     register_events();
            void     register_focus_controls(Control* control);
            void     process_mouse_move();
            bool     process_mouse_move(Control* control, const Point& position);
            Control* hover_control_at_position(const Point& position);
            Control* hover_control_at_position(Control* control, const Point& position);
            void     process_mouse_down();
            bool     process_mouse_down(Control* control, const Point& position);
            void     process_mouse_up();
            bool     process_mouse_up(Control* control, const Point& position);
            void     process_mouse_scroll(int16_t delta);
            bool     process_mouse_scroll(Control* control, const Point& position, int16_t delta);
            void     process_key_down(uint16_t key);
            bool     process_key_down(Control* control, uint16_t key);
            void     process_char(uint16_t key);
            bool     process_char(Control* control, uint16_t key);


            void     set_focus_control(Control* control);

            TokenStore      _token_store;
            input::Mouse    _mouse;
            input::Keyboard _keyboard;
            trview::Window  _window;

            Control&       _control;
            Control*       _hover_control{ nullptr };
            Control*       _focus_control{ nullptr };
        };
    }
}