/// @file Keyboard.h
/// @brief Class that receives keyboard input on a specific window.
/// 
/// Given a window will receive input messages and translate them into events that
/// can be subscribed to.

#pragma once

#include <cstdint>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>
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

            /// Destructor for Keyboard. This will remove the keyboard from the all keyboard
            /// map. This will stop messages being sent to this keyboard.
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

            /// Defines information required to correctly subclass the window and send messages
            /// to all relevant keyboards.
            struct KeyboardEntry
            {
                /// The window procedure to forward messages to once they have been processed.
                WNDPROC old_procedure{ nullptr };
                /// The keyboards associated with this window.
                std::vector<Keyboard*> keyboards;
            };

            /// Type definition for the static all keyboards map. The keyboard should refer to
            /// the member variable to access the keyboard map as the static instance may have
            /// been destroyed at the time that the keyboard is destroyed.
            using KeyboardMap = std::unordered_map<HWND, KeyboardEntry>;
        private:
            HWND _window;
            std::shared_ptr<KeyboardMap> _all_keyboards;
        };
    }
}
