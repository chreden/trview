#include "StackLayout.h"
#include "../Control.h"

namespace trview
{
    namespace ui
    {
        StackLayout::StackLayout(float padding, Direction direction, SizeMode size_mode)
            : _padding(padding), _direction(direction), _size_mode(size_mode)
        {
        }

        void StackLayout::bind(Control& control)
        {
            _control = &control;
            _token_store += control.on_add_child += [&](auto&& child)
            {
                child->set_position(get_next_position());
                _token_store += child->on_size_changed += [&](auto) { recalculate_layout(); };
                recalculate_layout();
            };
            _token_store += control.on_remove_child += [&](auto&&...) { recalculate_layout(); };
        }

        Point StackLayout::get_next_position() const
        {
            if (!_control)
            {
                return Point();
            }

            auto current_child_elements = _control->child_elements();
            if (!current_child_elements.empty())
            {
                auto last = current_child_elements.back();auto last_position = last->position();
                auto last_size = last->size();
                return get_next_position(last_position, last_size);
            }
            return Point(_margin.width, _margin.height);
        }

        Point StackLayout::get_next_position(Point previous_position, Size previous_size) const
        {
            return _direction == Direction::Vertical ?
                Point(previous_position.x, previous_position.y + previous_size.height) :
                Point(previous_position.x + previous_size.width, previous_position.y);
        }

        void StackLayout::recalculate_layout()
        {
            if (!_control)
            {
                return;
            }

            Point previous_position(_margin.width, _margin.height);
            Size previous_size(0, 0);
            float max_height = 0;
            float max_width = 0;
            bool first = true;

            for (const auto& element : _control->child_elements())
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
                    size.height = _control->size().height;
                }
                else if (_size_dimension == SizeDimension::Height)
                {
                    size.width = _control->size().width;
                }

                _control->set_size(size);
            }

            // Now that the size has been updated, update for alignment.
            if (_direction == Direction::Horizontal)
            {
                for (const auto& element : _control->child_elements())
                {
                    if (element->vertical_alignment() == Align::Centre)
                    {
                        const auto difference = (_control->size().height - element->size().height) / 2.0f;
                        element->set_position(Point(element->position().x, difference));
                    }
                }
            }
            else if (_direction == Direction::Vertical)
            {
                for (const auto& element : _control->child_elements())
                {
                    if (element->horizontal_alignment() == Align::Centre)
                    {
                        const auto difference = (_control->size().width - element->size().width) / 2.0f;
                        element->set_position(Point(difference, element->position().y));
                    }
                }
            }

            _control->on_invalidate();
        }

        Point StackLayout::get_padding() const
        {
            return _direction == Direction::Vertical ? Point(0, _padding) : Point(_padding, 0);
        }

        void StackLayout::set_auto_size_dimension(SizeDimension dimension)
        {
            _size_dimension = dimension;
            recalculate_layout();
        }

        void StackLayout::set_margin(const Size& margin)
        {
            _margin = margin;
            recalculate_layout();
        }

        SizeMode StackLayout::size_mode() const
        {
            return _size_mode;
        }

        float StackLayout::padding() const
        {
            return _padding;
        }

        StackLayout::Direction StackLayout::direction() const
        {
            return _direction;
        }

        Size StackLayout::margin() const
        {
            return _margin;
        }

        SizeDimension StackLayout::size_dimension() const
        {
            return _size_dimension;
        }
    }
}
