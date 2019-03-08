#pragma once

#include <trview.common/Window.h>

namespace trview
{
    namespace ui
    {
        class Control;

        /// Manages the mouse and keyboard input state for a control tree.
        class Input final
        {
        public:
            explicit Input(const trview::Window& window, Control& control);
        private:

        };
    }
}