#include "NumericUpDown.h"
#include "Label.h"
#include "Button.h"

namespace trview
{
    namespace ui
    {
        const std::string NumericUpDown::Names::up = "numeric_up_down_up";
        const std::string NumericUpDown::Names::down = "numeric_up_down_down";

        NumericUpDown::NumericUpDown(Size size, Colour background_colour, int32_t minimum, int32_t maximum)
            : NumericUpDown(Point(), size, background_colour, minimum, maximum)
        {
        }

        NumericUpDown::NumericUpDown(Point point, Size size, Colour background_colour, int32_t minimum, int32_t maximum)
            : Window(point, size, background_colour), _minimum(minimum), _maximum(maximum)
        {
            _label = add_child(std::make_unique<Label>(Size(size.width - 16, size.height), background_colour, L"0", 8, graphics::TextAlignment::Centre, graphics::ParagraphAlignment::Centre));
            auto up = add_child(std::make_unique<Button>(Point(size.width - 16, 0), Size(16, size.height / 2)));
            up->set_name(Names::up);
            up->set_background_colour(Colour::Transparent);
            auto down = add_child(std::make_unique<Button>(Point(size.width - 16, size.height / 2), Size(16, size.height / 2)));
            down->set_name(Names::down);
            down->set_background_colour(Colour::Transparent);

            up->set_border_thickness(0u);
            down->set_border_thickness(0u); 

            _token_store += up->on_click += [&]() { set_value(_value + 1); on_value_changed(_value); };
            _token_store += down->on_click += [&]() { set_value(_value - 1); on_value_changed(_value); };

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

        int32_t NumericUpDown::value() const
        {
            return _value;
        }
    }
}
