#define NOMINMAX

#include "NumericUpDown.h"
#include "Label.h"
#include "Button.h"

#include <algorithm>

namespace trview
{
    namespace ui
    {
        NumericUpDown::NumericUpDown(Point point, Size size, Colour background_colour, graphics::Texture up_texture, graphics::Texture down_texture, int32_t minimum, int32_t maximum)
            : Window(point, size, background_colour), _minimum(minimum), _maximum(maximum)
        {
            auto label = std::make_unique<Label>(Point(), Size(size.width - 16, size.height), background_colour, L"0", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre);
            auto up = std::make_unique<Button>(Point(size.width - 16, 0), Size(16, size.height / 2), up_texture, up_texture);
            auto down = std::make_unique<Button>(Point(size.width - 16, size.height / 2), Size(16, size.height / 2), down_texture, down_texture);

            up->set_border_thickness(0u);
            down->set_border_thickness(0u);

            _token_store += up->on_click += [&]() { set_value(_value + 1); on_value_changed(_value); };
            _token_store += down->on_click += [&]() { set_value(_value - 1); on_value_changed(_value); };

            _label = add_child(std::move(label));
            add_child(std::move(up));
            add_child(std::move(down));

            set_value(_minimum);
        }

        void NumericUpDown::set_value(int32_t value)
        {
            auto new_value = std::max(std::min(value, _maximum), _minimum);
            if (_value == new_value)
            {
                return;
            }

            _value = value;
            _label->set_text(std::to_wstring(value));
            on_invalidate();
        }

        void NumericUpDown::set_maximum(int32_t maximum)
        {
            _maximum = maximum;
            set_value(_value);
        }
    }
}
