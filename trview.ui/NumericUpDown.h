#pragma once

#include "Window.h"
#include <trview.common/Event.h>

namespace trview
{
    namespace ui
    {
        class NumericUpDown : public Window
        {
        public:
            NumericUpDown(Point point, Size size);

            Event<int32_t> on_value_changed;
        };
    }
}