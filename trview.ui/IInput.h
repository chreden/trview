#pragma once

#include <trview.input/IMouse.h>
#include <trview.common/Window.h>

namespace trview
{
    namespace ui
    {
        class Control;

        struct IInput
        {
            using Source = std::function<std::unique_ptr<IInput>(const trview::Window, Control&)>;

            virtual ~IInput() = 0;

            /// Get the control that is currently focused.
            /// @returns The control, or nullptr if no control is focused.
            virtual Control* focus_control() const = 0;

            virtual input::IMouse& mouse() = 0;
        };
    }
}