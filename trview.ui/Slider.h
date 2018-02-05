#pragma once

#include "Window.h"
#include <trview.common/Event.h>

namespace trview
{
    namespace ui
    {
        class Slider : public Window
        {
        public:
            Slider(Point position, Size size);
            virtual ~Slider() = default;

            Event<float> on_value_changed;
        protected:
            
        private:
            float _value{ 0.5f };
        };
    }
}