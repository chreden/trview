#include "Input.h"
#include "Control.h"
#include <stack>

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

        Input::Input(const trview::Window& window, Control& control)
            : _mouse(window), _window(window), _control(control)
        {
            _token_store += _mouse.mouse_move += [&](auto, auto) { process_mouse_move(); };
        }

        void Input::process_mouse_move()
        {
            auto position = client_cursor_position(_window);

            Control* control = hover_control_at_position(position);
            if (control != _hover_control)
            {
                if (_hover_control)
                {
                    _hover_control->mouse_leave();
                }
                _hover_control = control;
                if (_hover_control)
                {
                    _hover_control->mouse_enter();
                }
            }

            // Now do movement.
            if (_focus_control)
            {
                auto focus = _focus_control;
                if (focus->move(position - focus->absolute_position()))
                {
                    return;
                }
            }

            process_mouse_move(&_control, position - _control.position());
        }

        Control* Input::hover_control_at_position(const Point& position)
        {
            return hover_control_at_position(&_control, position);
        }

        Control* Input::hover_control_at_position(Control* control, const Point& position)
        {
            if (!control->visible() || !in_bounds(position, control->size()))
            {
                return nullptr;
            }

            for (const auto& child : control->child_elements())
            {
                auto result = hover_control_at_position(child, position - child->position());
                if (result && result->_handles_hover)
                {
                    return result;
                }
            }

            if (control->_handles_hover)
            {
                return control;
            }

            return nullptr;
        }

        bool Input::process_mouse_move(Control* control, const Point& position)
        {
            // Bounds check - before child elements are checked.
            if (!control->visible() || !in_bounds(position, control->size()))
            {
                return false;
            }

            for (auto& child : control->child_elements())
            {
                // Convert the position into the coordinate space of the child element.
                if (process_mouse_move(child, position - child->position()))
                {
                    return true;
                }
            }

            // If none of the child elements have handled this event themselves, call the 
            // move function of this control.
            return control->move(position);
        }
    }
}