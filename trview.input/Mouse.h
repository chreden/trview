/// @file Mouse.h
/// @brief Class that receives mouse input on a specific window.
/// 
/// Given a window will receive input messages and translate them into events that
/// can be subscribed to.

#pragma once

#include <windows.h>
#include <trview.common/Event.h>
#include <trview.common/MessageHandler.h>
#include <cstdint>

namespace trview
{
    namespace input
    {
        /// Receives mouse input on a specific window.
        class Mouse : public MessageHandler
        {
        public:
            /// The mouse button that was pressed.
            enum class Button
            {
                /// The left mouse button.
                Left,
                /// The right mouse button.
                Right
            };

            /// Creates an instance of the Mouse class.
            /// @param window The window to monitor.
            explicit Mouse(HWND window);

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
            /// @param window The window that received the message.
            /// @param message The message that was received.
            /// @param wParam The WPARAM for the message.
            /// @param lParam The LPARAM for the message.
            virtual void process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam) override;
        private:
            void raise_absolute_mouse_move(long x, long y);

            bool _any_absolute_previous{ false };
            long _absolute_x;
            long _absolute_y;
        };
    }
}