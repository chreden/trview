#include "GroupBox.h"
#include "Label.h"

namespace trview
{
    namespace ui
    {
        GroupBox::GroupBox(const Size& size, const Colour& background_colour, const Colour& border_colour, const std::wstring& text)
            : GroupBox(Point(), size, background_colour, border_colour, text)
        {
        }

        GroupBox::GroupBox(const Point& point, const Size& size, const Colour& background_colour, const Colour& border_colour, const std::wstring& text)
            : Window(point, size, background_colour), _border_colour(border_colour)
        {
            auto top_left = std::make_unique<Window>(Point(0, 5), Size(9, 2), border_colour);
            auto top_right = std::make_unique<Window>(Point(0, 5), Size(size.width, 2), border_colour);
            auto left = std::make_unique<Window>(Point(0, 5), Size(2, size.height - 10), border_colour);
            auto bottom = std::make_unique<Window>(Point(0, size.height - 2 - 5), Size(size.width, 2), border_colour);
            auto right = std::make_unique<Window>(Point(size.width - 2, 5), Size(2, size.height - 10), border_colour);
            auto label = std::make_unique<Label>(Point(10, 0), Size(55, 20), background_colour, text, 8, graphics::TextAlignment::Left, graphics::ParagraphAlignment::Near, SizeMode::Auto);

            add_child(std::move(top_left));
            _top_right = add_child(std::move(top_right));
            add_child(std::move(left));
            add_child(std::move(bottom));
            add_child(std::move(right));
            _label = add_child(std::move(label));

            _token_store += _label->on_size_changed += [&](const auto& new_size)
            {
                // Adjust top right bar to be the right position and size.
                _top_right->set_position(_label->position() + Point(new_size.width + 1, 5));
                _top_right->set_size(Size(this->size().width - _top_right->position().x - 1, 2));
            };

            // Inner area.
            _area = add_child(std::make_unique<Window>(Point(10, 21), size - Size(10, 21), background_colour));
        }

        std::wstring GroupBox::title() const
        {
            return _label->text();
        }

        void GroupBox::set_title(const std::wstring& title)
        {
            _label->set_text(title);
        }

        void GroupBox::inner_add_child(Control* child_element)
        {
            if (_area == nullptr)
            {
                return;
            }

            auto child = remove_child(child_element);
            _area->add_child(std::move(child));
        }

        Colour GroupBox::border_colour() const
        {
            return _border_colour;
        }
    }
}
