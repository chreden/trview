#pragma once

#include <cstdint>
#include <memory>

#include <trview.common/Event.h>
#include <trview.common/Window.h>

namespace trview
{
    namespace input
    {
        struct IMouse
        {
            using Source = std::function<std::shared_ptr<IMouse>(const Window& window)>;

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

            virtual ~IMouse() = 0;

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
            virtual long x() const = 0;

            /// Get the current Y coordinate of the mouse.
            virtual long y() const = 0;
        };
    }
}
