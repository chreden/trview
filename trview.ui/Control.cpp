#include "Control.h"

#include <algorithm>

namespace trview
{
    namespace ui
    {
        Control::Control(Point position, Size size)
            : _position(position), _size(size), _visible(true)
        {
        }

        Control::~Control()
        {
        }

        Point Control::position() const
        {
            return _position;
        }

        Size Control::size() const
        {
            return _size;
        }

        bool Control::visible() const
        {
            return _visible;
        }

        void Control::set_visible(bool value)
        {
            _visible = value;
        }

        Control* Control::parent() const
        {
            return _parent;
        }

        void Control::add_child(std::unique_ptr<Control>&& child_element)
        {
            child_element->_parent = this;
            _child_elements.push_back(std::move(child_element));
        }

        std::vector<Control*> Control::child_elements() const
        {
            std::vector<Control*> output;
            std::transform(_child_elements.begin(), _child_elements.end(),
                std::back_inserter(output), [](auto& child) {return child.get(); });
            return output;
        }

        bool Control::mouse_down(Point position)
        {
            // Bounds check - before child elements are checked.
            if (!(position.x >= 0 && position.y >= 0 && position.x <= _size.width && position.y <= _size.height))
            {
                return false;
            }

            bool handled = false;
            for (auto& child : _child_elements)
            {
                // Convert the position into the coordinate space of the child element.
                handled |= child->mouse_down(position - child->position());
            }

            // If none of the child elements have handled this event themselves, call the 
            // clicked function of this control.
            if (!handled)
            {
                clicked();
            }

            return true;
        }

        void Control::clicked()
        {
        }
    }
}