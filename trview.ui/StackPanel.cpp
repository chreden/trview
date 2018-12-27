#define NOMINMAX
#include "StackPanel.h"

namespace trview
{
    namespace ui
    {
        StackPanel::StackPanel(Point position, Size size, Colour colour, Size padding, Direction direction, SizeMode size_mode)
            : Window(position, size, colour), _padding(padding), _direction(direction), _size_mode(size_mode)
        {
        }

        void StackPanel::inner_add_child(Control* child_element)
        {
            child_element->set_position(get_next_position());
            _token_store.add(child_element->on_size_changed += [&](auto) { recalculate_layout(); });
            recalculate_layout();
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
            return Point(_margin.width, _margin.height);
        }

        Point StackPanel::get_next_position(Point previous_position, Size previous_size) const
        {
            return _direction == Direction::Vertical ?
                Point(previous_position.x, previous_position.y + previous_size.height) :
                Point(previous_position.x + previous_size.width, previous_position.y);
        }

        Point StackPanel::get_padding() const
        {
            return _direction == Direction::Vertical ? Point(0, _padding.height) : Point(_padding.width, 0);
        }

        void StackPanel::recalculate_layout()
        {
            Point previous_position(_margin.width, _margin.height);
            Size previous_size(0,0);
            float max_height = 0;
            float max_width = 0;
            bool first = true;

            for (const auto& element : child_elements())
            {
                auto position = get_next_position(previous_position, previous_size);
                if (!first)
                {
                    position += get_padding();
                }
                element->set_position(position);
                previous_position = position;
                previous_size = element->size();

                max_width = std::max(previous_size.width, max_width);
                max_height = std::max(previous_size.height, max_height);

                first = false;
            }

            if (_size_mode == SizeMode::Auto)
            {
                auto size = Size(previous_position.x + _margin.width, previous_position.y + _margin.height);

                if (_direction == Direction::Horizontal)
                {
                    size += Size(previous_size.width, max_height);
                }
                else if (_direction == Direction::Vertical)
                {
                    size += Size(max_width, previous_size.height);
                }

                if (_size_dimension == SizeDimension::Width)
                {
                    size.height = this->size().height;
                }
                else if (_size_dimension == SizeDimension::Height)
                {
                    size.width = this->size().width;
                }

                set_size(size);
            }

            // Now that the size has been updated, update for alignment.
            if (_direction == Direction::Horizontal)
            {
                for (const auto& element : child_elements())
                {
                    if (element->vertical_alignment() == Align::Centre)
                    {
                        const auto difference = (size().height - element->size().height) / 2.0f;
                        element->set_position(Point(element->position().x, difference));
                    }
                }
            }
            else if (_direction == Direction::Vertical)
            {
                for (const auto& element : child_elements())
                {
                    if (element->horizontal_alignment() == Align::Centre)
                    {
                        const auto difference = (size().width - element->size().width) / 2.0f;
                        element->set_position(Point(difference, element->position().y));
                    }
                }
            }

            on_invalidate();
        }

        void StackPanel::set_auto_size_dimension(SizeDimension dimension)
        {
            _size_dimension = dimension;
            recalculate_layout();
        }

        void StackPanel::set_margin(const Size& margin)
        {
            _margin = margin;
            recalculate_layout();
        }
    }
}
