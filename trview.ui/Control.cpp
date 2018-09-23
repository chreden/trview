#include "Control.h"

#include <algorithm>
#include <iterator>

namespace trview
{
    namespace ui
    {
        namespace
        {
            bool in_bounds(const Point& position, const Size& size)
            {
                return position.x >= 0 && position.y >= 0 && position.x <= size.width && position.y <= size.height;
            }
        }

        Control::Control(Point position, Size size)
            : _position(position), _size(size), _visible(true)
        {
        }

        Control::~Control()
        {
        }

        Align Control::horizontal_alignment() const
        {
            return _horizontal_alignment;
        }

        Align Control::vertical_alignment() const
        {
            return _vertical_alignment;
        }

        void Control::set_vertical_alignment(Align mode)
        {
            _vertical_alignment = mode;
            on_size_changed(_size);
        }

        void Control::set_horizontal_alignment(Align mode)
        {
            _horizontal_alignment = mode;
            on_size_changed(_size);
        }

        Point Control::position() const
        {
            return _position;
        }

        Point Control::absolute_position() const
        {
            if (_parent)
            {
                return _position + _parent->absolute_position();
            }
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
            if (_visible == value)
            {
                return;
            }
            _visible = value;
            on_invalidate();
        }

        Control* Control::parent() const
        {
            return _parent;
        }

        void Control::clear_child_elements()
        {
            _child_elements.clear();
            on_hierarchy_changed();
            on_invalidate();
        }

        std::vector<Control*> Control::child_elements() const
        {
            std::vector<Control*> output;
            std::transform(_child_elements.begin(), _child_elements.end(),
                std::back_inserter(output), [](auto& child) {return child.get(); });
            return output;
        }

        bool Control::process_mouse_down(const Point& position)
        {
            // Bounds check - before child elements are checked.
            if (!visible() || !in_bounds(position, _size))
            {
                return false;
            }

            bool handled = false;
            for (auto& child : _child_elements)
            {
                // Convert the position into the coordinate space of the child element.
                handled |= child->process_mouse_down(position - child->position());
            }

            // If none of the child elements have handled this event themselves, call the 
            // mouse_down function of this control.
            if (handled)
            {
                return true;
            }

            // Promote controls to focus control, or clear if there are no controls that 
            // accepted the event.
            bool handled_by_self = _handles_input && mouse_down(position);
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

        bool Control::process_mouse_move(const Point& position)
        {
            if (_focus_control && _focus_control != this)
            {
                bool focus_handled = _focus_control->move(position - _focus_control->absolute_position());
                if (focus_handled)
                {
                    return true;
                }
            }

            // Get the control at the current mouse position.
            Control* control = hover_control_at_position(position);
            if (control != _hover_control)
            {
                if (_hover_control)
                {
                    _hover_control->mouse_leave();
                }
                _hover_control = control;
                _hover_control->mouse_enter();
            }

            return inner_process_mouse_move(position);
        }

        bool Control::inner_process_mouse_move(const Point& position)
        {
            // Bounds check - before child elements are checked.
            if (!in_bounds(position, _size))
            {
                return false;
            }

            bool handled = false;
            for (auto& child : _child_elements)
            {
                // Convert the position into the coordinate space of the child element.
                handled |= child->inner_process_mouse_move(position - child->position());
            }

            // If none of the child elements have handled this event themselves, call the 
            // move function of this control.
            return handled | move(position);
        }

        bool Control::process_mouse_up(const Point& position)
        {
            if (_focus_control && _focus_control != this)
            {
                const auto control_space_position = position - _focus_control->absolute_position();
                bool focus_handled = _focus_control->mouse_up(control_space_position);
                if (focus_handled && in_bounds(control_space_position, _focus_control->size()))
                {
                    _focus_control->clicked(control_space_position);
                }

                set_focus_control(nullptr);

                if (focus_handled)
                {
                    return true;
                }
            }
            return inner_process_mouse_up(position);
        }

        bool Control::inner_process_mouse_up(const Point& position)
        {
            // Bounds check - before child elements are checked.
            if (!in_bounds(position, _size))
            {
                return false;
            }

            bool handled = false;
            for (auto& child : _child_elements)
            {
                // Convert the position into the coordinate space of the child element.
                handled |= child->inner_process_mouse_up(position - child->position());
            }

            // If none of the child elements have handled this event themselves, call the up of the control.
            return handled | mouse_up(position);
        }

        bool Control::mouse_scroll(const Point& position, int16_t delta)
        {
            if (_focus_control && _focus_control != this)
            {
                bool focus_handled = _focus_control->mouse_scroll(position, delta);
                if (focus_handled)
                {
                    return true;
                }
            }

            // Bounds check - before child elements are checked.
            if (!in_bounds(position, _size))
            {
                return false;
            }

            bool handled = false;
            for (auto& child : _child_elements)
            {
                // Convert the position into the coordinate space of the child element.
                handled |= child->mouse_scroll(position - child->position(), delta);
            }

            // If none of the child elements have handled this event themselves, call the 
            // move function of this control.
            return handled | scroll(delta);
        }

        bool Control::mouse_down(const Point&)
        {
            return false;
        }

        bool Control::mouse_up(const Point&)
        {
            return false;
        }

        bool Control::clicked(Point)
        {
            return false;
        }

        bool Control::move(Point)
        {
            return false;
        }

        bool Control::scroll(int)
        {
            return false;
        }

        bool Control::key_down(uint16_t)
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
                inner_set_focus_control(control);
            }
        }

        void Control::inner_set_focus_control(Control* control)
        {
            _focus_control = control;
            for (const auto& child : _child_elements)
            {
                child->inner_set_focus_control(control);
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

        bool Control::process_key_down(uint16_t key)
        {
            if (_focus_control && _focus_control != this)
            {
                bool focus_handled = _focus_control->key_down(key);
                if (focus_handled)
                {
                    return true;
                }
            }

            return inner_process_key_down(key);
        }

        bool Control::inner_process_key_down(uint16_t key)
        {
            bool handled = false;
            for (auto& child : _child_elements)
            {
                if (child->inner_process_key_down(key))
                {
                    return handled;
                }
            }

            // If none of the child elements have handled this event themselves, call the key_down
            // event of the control.
            return key_down(key);
        }

        bool Control::is_mouse_over(const Point& position) const
        {
            if (!in_bounds(position, _size))
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

        const std::string& Control::name() const
        {
            return _name;
        }

        void Control::set_name(const std::string& name)
        {
            _name = name;
        }

        void Control::inner_add_child(Control*)
        {
        }

        Control* Control::hover_control_at_position(const Point& position)
        {
            if (!in_bounds(position, size()))
            {
                return nullptr;
            }

            for (const auto& control : _child_elements)
            {
                auto result = control->hover_control_at_position(position - control->position());
                if (result && result->_handles_hover)
                {
                    return result;
                }
            }

            if (_handles_hover)
            {
                return this;
            }

            return nullptr;
        }

        bool Control::mouse_enter()
        {
            return false;
        }

        bool Control::mouse_leave()
        {
            return false;
        }

        void Control::set_hover_control(Control* control)
        {
            if (_parent)
            {
                _parent->set_hover_control(control);
            }
            else
            {
                inner_set_hover_control(control);
            }
        }

        void Control::inner_set_hover_control(Control* control)
        {
            _hover_control = control;
            for (const auto& child : _child_elements)
            {
                child->inner_set_hover_control(control);
            }
        }

        void Control::set_handles_hover(bool value)
        {
            _handles_hover = value;
        }
    }
}