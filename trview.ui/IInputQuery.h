#pragma once

namespace trview
{
    namespace ui
    {
        class Control;

        /// Allows controls to query the current state of user input.
        struct IInputQuery
        {
            virtual ~IInputQuery() = 0;

            /// Get the control that is currently focused.
            /// @returns The control, or nullptr if no control is focused.
            virtual Control* focus_control() const = 0;
        };
    }
}