#include "StackPanel.h"

namespace trview
{
    namespace ui
    {
        StackPanel::StackPanel(Point position, Size size, Colour colour, Size padding)
            : Window(position, size, colour), _padding(padding)
        {
        }

        void StackPanel::add_child(std::unique_ptr<Control>&& child_element)
        {
            child_element->set_position(get_next_position());
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
                return Point(_padding.width, last_position.y + last_size.height + _padding.height);
            }
            return Point(_padding.width, _padding.height);
        }
    }
}
