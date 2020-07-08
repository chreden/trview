/// @file Keyboard.h
/// @brief Class that receives keyboard input on a specific window.
/// 
/// Given a window will receive input messages and translate them into events that
/// can be subscribed to.

#pragma once

#include <cstdint>
#include <trview.common/Event.h>
#include <trview.common/MessageHandler.h>

namespace trview
{
    namespace input
    {
        /// Receives keyboard input for a specific window.
        class Keyboard : public MessageHandler
        {
        public:
            /// Create a keyboard to listen to keyboard messages for a specific window.
            /// @param window The window to listen to.
            explicit Keyboard(const Window& window);

            /// Destructor for Keyboard. This will remove the keyboard from the all keyboard
            /// map. This will stop messages being sent to this keyboard.
            virtual ~Keyboard();

            /// Determines whether the control key is currently pressed.
            /// @returns Whether control is currently pressed.
            bool control() const;

            /// Determines whether the shift key is currently pressed.
            /// @returns Whether shift is currently pressed.
            bool shift() const;

            /// Event raised when the user presses a key. The key is passed as a parameter
            /// to the event listeners.
            Event<uint16_t, bool, bool> on_key_down;

            /// Event raised when the user releases a key. The key is passed as a parameter
            /// to the event listeners.
            Event<uint16_t, bool, bool> on_key_up;

            /// Event raised when the user enters a text character. The character is passed as
            /// a parameter to the event listener.
            Event<uint16_t> on_char;

            /// Handles a window message.
            /// @param message The message that was received.
            /// @param wParam The WPARAM for the message.
            /// @param lParam The LPARAM for the message.
            virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        };
    }
}
