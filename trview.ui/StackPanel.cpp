#include "StackPanel.h"

namespace trview
{
    namespace ui
    {
        StackPanel::StackPanel(Point position, Size size, Colour colour, Size padding, Direction direction, SizeMode size_mode)
            : Window(position, size, colour), _padding(padding), _direction(direction), _size_mode(size_mode)
        {
        }

        void StackPanel::add_child(std::unique_ptr<Control>&& child_element)
        {
            child_element->set_position(get_next_position());
            child_element->on_size_changed += [&](auto) { recalculate_layout(); };
            Window::add_child(std::move(child_element));
        }

        Point StackPanel::get_next_position() const
        {
            auto current_child_elements = child_elements();
            if (!current_child_elements.empty())
            {
                auto last = current_child_elements.back();
                auto last_position = last->position();
                auto last_size = last->size();
                return get_next_position(last_position, last_size);
            }
            return Point(_padding.width, _padding.height);
        }

        Point StackPanel::get_next_position(Point previous_position, Size previous_size) const
        {
            return _direction == Direction::Vertical ?
                Point(_padding.width, previous_position.y + previous_size.height + _padding.height) :
                Point(previous_position.x + previous_size.width + _padding.width, _padding.height);
        }

        void StackPanel::recalculate_layout()
        {
            Point previous_position(_padding.width, _padding.height);
            Size previous_size(0,0);

            for (const auto& element : child_elements())
            {
                auto position = get_next_position(previous_position, previous_size);
                element->set_position(position);
                previous_position = position;
                previous_size = element->size();
            }

            if (_size_mode == SizeMode::Auto)
            {
                set_size(
                    Size(previous_position.x + previous_size.width + _padding.width,
                         previous_position.y + previous_size.height + _padding.height));
            }
        }
    }
}
