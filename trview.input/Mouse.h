/// @file Mouse.h
/// @brief Class that receives mouse input on a specific window.
/// 
/// Given a window will receive input messages and translate them into events that
/// can be subscribed to.

#pragma once

#include <trview.common/Event.h>
#include <trview.common/MessageHandler.h>
#include <cstdint>

#include "IWindowTester.h"

namespace trview
{
    namespace input
    {
        /// Receives mouse input on a specific window.
        class Mouse final : public MessageHandler
        {
        public:
            /// The mouse button that was pressed.
            enum class Button
            {
                /// The left mouse button.
                Left,
                /// The middle mouse button.
                Middle,
                /// The right mouse button.
                Right
            };

            /// Creates an instance of the Mouse class.
            /// @param window The window to monitor.
            /// @param window_tester The window tester to use.
            explicit Mouse(const Window& window, std::unique_ptr<IWindowTester>&& window_tester);

            /// Destructor for Mouse. This will remove the mouse from the all mice
            /// map. This will stop messages being sent to this mouse.
            virtual ~Mouse();

            /// Process the specified raw input data. This will raise any appropriate events.
            /// @param input The mouse raw input data.
            /// @remarks This is called internally and does not need to be called from external sources.
            void process_input(const RAWINPUT& input);

            /// Event raised when a mouse button is pressed. The button that was pressed is passed
            /// as a parameter to the event listeners.
            Event<Button> mouse_down;

            /// Event raised when a mouse button is released. The button that was released is passed
            /// as a parameter to the event listeners.
            Event<Button> mouse_up;

            /// Event raised when a mouse is pressed and released (clicked). The button that was clicked
            /// is passed as a parameter to the event listeners.
            Event<Button> mouse_click;

            /// Event raised when the mouse wheel has rotated. The amount that the wheel has rotated
            /// is passed as a parameter to the event listeners.
            Event<int16_t> mouse_wheel;

            /// Event raised when the mouse has moved. The difference in X and Y coordinates are passed
            /// as the parameters to the event listeners.
            Event<long, long> mouse_move;

            /// Get the current X coordinate of the mouse.
            long x() const;

            /// Get the current Y coordinate of the mouse.
            long y() const;

            /// Handles a window message.
            /// @param message The message that was received.
            /// @param wParam The WPARAM for the message.
            /// @param lParam The LPARAM for the message.
            virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        private:
            void raise_absolute_mouse_move(long x, long y);

            /// Update the state of the specified button and raise the appropriate events.
            /// @param button The button to use in any events raised.
            /// @flags The raw input flags to process.
            /// @down_mask The flag mask for the button being down.
            /// @up_mask The flag mask for the button being up.
            /// @param last_down The timing variable to update to determine whether there was a click.
            void update_button(Button button, USHORT flags, USHORT down_mask, USHORT up_mask, DWORD& last_down);

            std::unique_ptr<IWindowTester> _window_tester;
            bool _any_absolute_previous{ false };
            long _absolute_x;
            long _absolute_y;

            DWORD _left_down{ 0u };
            DWORD _middle_down{ 0u };
            DWORD _right_down{ 0u };
        };
    }
}