#pragma once

#include <trview.input/IMouse.h>

namespace trview
{
    namespace ui
    {
        class Control;

        struct IInput
        {
            virtual ~IInput() = 0;

            /// Get the control that is currently focused.
            /// @returns The control, or nullptr if no control is focused.
            virtual Control* focus_control() const = 0;

            virtual input::IMouse& mouse() = 0;
        };
    }
}