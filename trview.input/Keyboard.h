/// @file Keyboard.h
/// @brief Class that receives keyboard input on a specific window.
/// 
/// Given a window will receive input messages and translate them into events that
/// can be subscribed to.

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
        /// Receives keyboard input for a specific window.
        class Keyboard
        {
        public:
            /// Create a keyboard to listen to keyboard messages for a specific window.
            /// @param window The window to listen to.
            explicit Keyboard(HWND window);

            ~Keyboard();

            /// Determines whether the control key is currently pressed.
            /// @returns Whether control is currently pressed.
            bool control() const;

            /// Event raised when the user presses a key. The key is passed as a parameter
            /// to the event listeners.
            Event<uint16_t> on_key_down;

            /// Event raised when the user releases a key. The key is passed as a parameter
            /// to the event listeners.
            Event<uint16_t> on_key_up;

            /// Event raised when the user enters a text character. The character is passed as
            /// a parameter to the event listener.
            Event<uint16_t> on_char;
        private:
            HWND _window;
        };
    }
}
