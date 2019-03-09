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
            on_deleting();
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

        std::vector<Control*> Control::child_elements(bool rendering_order) const
        {
            std::vector<Control*> output;
            std::transform(_child_elements.begin(), _child_elements.end(),
                std::back_inserter(output), [](auto& child) {return child.get(); });
            if (rendering_order)
            {
                std::sort(output.begin(), output.end(), [](const auto& l, const auto& r) { return l->z() > r->z(); });
            }
            else
            {
                std::sort(output.begin(), output.end(), [](const auto& l, const auto& r) { return l->z() < r->z(); });
            }
            return output;
        }

        bool Control::mouse_scroll(const Point& position, int16_t delta)
        {
            if (_focus_control && _focus_control != this)
            {
                if (_focus_control->inner_process_mouse_scroll(position, delta))
                {
                    return true;
                }
            }
            return inner_process_mouse_scroll(position, delta);
        }

        bool Control::inner_process_mouse_scroll(const Point& position, int delta)
        {
            // Bounds check - before child elements are checked.
            if (!visible() || !in_bounds(position, _size))
            {
                return false;
            }

            for (auto& child : child_elements())
            {
                // Convert the position into the coordinate space of the child element.
                if (child->inner_process_mouse_scroll(position - child->position(), delta))
                {
                    return true;
                }
            }

            // If none of the child elements have handled this event themselves, call the 
            // scroll function of this control.
            return scroll(delta);
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

        void Control::clicked_off(Control*)
        {
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

        bool Control::key_char(wchar_t key)
        {
            return false;
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
                if (_focus_control->key_down(key))
                {
                    return true;
                }
            }

            return inner_process_key_down(key);
        }

        bool Control::inner_process_key_down(uint16_t key)
        {
            if (!visible())
            {
                return false;
            }

            for (auto& child : child_elements())
            {
                if (child->inner_process_key_down(key))
                {
                    return true;
                }
            }

            // If none of the child elements have handled this event themselves, call the key_down
            // event of the control.
            return key_down(key);
        }

        bool Control::process_char(wchar_t key)
        {
            if (_focus_control && _focus_control != this && _focus_control->key_char(key))
            {
                return true;
            }
            return inner_process_char(key);
        }

        bool Control::inner_process_char(wchar_t key)
        {
            if (!visible())
            {
                return false;
            }

            for (auto& child : child_elements())
            {
                if (child->inner_process_char(key))
                {
                    return true;
                }
            }
            return key_char(key);
        }

        bool Control::is_mouse_over(const Point& position) const
        {
            if (!visible() || !in_bounds(position, _size))
            {
                return false;
            }

            bool is_over_child = false;
            for (const auto& child : child_elements())
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

        void Control::mouse_enter()
        {
        }

        void Control::mouse_leave()
        {
        }

        void Control::set_handles_hover(bool value)
        {
            _handles_hover = value;
        }

        int Control::z() const
        {
            return _z;
        }

        void Control::set_z(int value)
        {
            _z = value;
            on_invalidate();
        }

        void Control::remove_child(Control* child_element)
        {
            _child_elements.erase(std::remove_if(_child_elements.begin(), _child_elements.end(), [&](const auto& element) { return element.get() == child_element; }), _child_elements.end());
            on_hierarchy_changed();
        }
    }
}