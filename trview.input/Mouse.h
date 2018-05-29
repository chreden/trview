/// @file Mouse.h
/// @brief Class that receives mouse input on a specific window.
/// 
/// Given a window will receive input messages and translate them into events that
/// can be subscribed to.

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
        /// Receives mouse input on a specific window.
        class Mouse
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
            ~Mouse();

            /// Process the specified raw input data. This will raise any appropriate events.
            /// @param input The mouse raw input data.
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