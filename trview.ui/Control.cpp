#include "Control.h"

#include <algorithm>
#include <iterator>

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

        void Control::set_position(Point position)
        {
            _position = position;
            on_invalidate();
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
            on_invalidate();
        }

        Control* Control::parent() const
        {
            return _parent;
        }

        void Control::add_child(std::unique_ptr<Control>&& child_element)
        {
            child_element->_parent = this;
            _child_elements.push_back(std::move(child_element));
            on_invalidate();
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
            if (handled)
            {
                return true;
            }

            // Promote controls to focus control, or clear if there are no controls that 
            // accepted the event.
            bool handled_by_self = clicked(position);
            if (handled_by_self)
            {
                set_focus_control(this);
            }
            else if (!_parent)
            {
                set_focus_control(nullptr);
            }
            return handled_by_self;
        }

        bool Control::mouse_move(Point position)
        {
            if (_focus_control && _focus_control != this)
            {
                bool focus_handled = _focus_control->mouse_move(position);
                if (focus_handled)
                {
                    return true;
                }
            }

            // Bounds check - before child elements are checked.
            if (!(position.x >= 0 && position.y >= 0 && position.x <= _size.width && position.y <= _size.height))
            {
                return false;
            }

            bool handled = false;
            for (auto& child : _child_elements)
            {
                // Convert the position into the coordinate space of the child element.
                handled |= child->mouse_move(position - child->position());
            }

            // If none of the child elements have handled this event themselves, call the 
            // move function of this control.
            return handled | move(position);
        }

        bool Control::mouse_up(Point)
        {
            set_focus_control(nullptr);
            return true;
        }

        bool Control::clicked(Point)
        {
            return false;
        }

        bool Control::move(Point)
        {
            return false;
        }

        void Control::set_focus_control(Control* control)
        {
            if (_parent)
            {
                _parent->set_focus_control(control);
            }
            else
            {
                _focus_control = control;
            }
        }

        Control* Control::focus_control() const
        {
            if (_parent)
            {
                return _parent->focus_control();
            }
            return _focus_control;
        }

        // Set whether this control handles input when tested in is_mouse_over. Defaults to true.
        // value: Whether the control handles input.
        void Control::set_handles_input(bool value)
        {
            _handles_input = value;
        }

        bool Control::is_mouse_over(Point position) const
        {
            if (!(position.x >= 0 && position.y >= 0 && position.x <= _size.width && position.y <= _size.height))
            {
                return false;
            }

            bool is_over_child = false;
            for (const auto& child : _child_elements)
            {
                if (child->visible())
                {
                    is_over_child |= child->is_mouse_over(position - child->position());
                }
            }

            return _handles_input || is_over_child;
        }

        void Control::set_size(Size size)
        {
            _size = size;
            on_size_changed(size);
            on_invalidate();
        }
    }
}