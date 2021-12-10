#pragma once

#include "Window.h"

namespace trview
{
    namespace ui
    {
        class Label;

        class NumericUpDown : public Window
        {
        public:
            struct Names
            {
                static const std::string up;
                static const std::string down;
                static const std::string value;
            };

            NumericUpDown(Size size, Colour background_colour, int32_t minimum, int32_t maximum);
            NumericUpDown(Point point, Size size,  Colour background_colour, int32_t minimum, int32_t maximum);
            Event<int32_t> on_value_changed;
            void set_value(int32_t value);
            void set_maximum(int32_t maximum);
            void set_minimum(int32_t minimum);
            int32_t value() const;
            int32_t min_value() const;
            int32_t max_value() const;
        private:
            int32_t _minimum;
            int32_t _maximum;
            int32_t _value{ 0 };

            ui::Label* _label;
        };
    }
}