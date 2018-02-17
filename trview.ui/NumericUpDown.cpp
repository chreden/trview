#define NOMINMAX

#include "NumericUpDown.h"
#include "Label.h"
#include "Button.h"

#include <sstream>
#include <algorithm>

namespace trview
{
    namespace ui
    {
        NumericUpDown::NumericUpDown(Point point, Size size, Colour background_colour, Texture up_texture, Texture down_texture, int32_t minimum, int32_t maximum)
            : Window(point, size, background_colour), _minimum(minimum), _maximum(maximum)
        {
            auto label = std::make_unique<Label>(Point(0, 0), Size(size.width - 16, size.height), background_colour, L"0", 10.f, TextAlignment::Centre, ParagraphAlignment::Centre);
            auto up = std::make_unique<Button>(Point(size.width - 16, 0), Size(16, size.height / 2), down_texture, down_texture);
            auto down = std::make_unique<Button>(Point(size.width - 16, size.height / 2), Size(16, size.height / 2), up_texture, up_texture);

            up->on_click += [&]() { set_value(_value + 1); };
            down->on_click += [&]() { set_value(_value - 1); };

            _label = label.get();

            add_child(std::move(label));
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

            std::wstringstream stream;
            stream << value;

            _label->set_text(stream.str());
            on_value_changed.raise(_value);
        }
    }
}
