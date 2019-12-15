#include "Input.h"
#include "Control.h"
#include <trview.input/WindowTester.h>

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
            : _mouse(window, std::make_unique<input::WindowTester>(window)), _keyboard(window), _window(window), _control(control)
        {
            register_events();
        }

        Control* Input::focus_control() const
        {
            return _focus_control;
        }

        void Input::register_events()
        {
            _token_store = TokenStore();

            register_focus_controls(&_control);

            _token_store += _mouse.mouse_move += [&](auto, auto) { process_mouse_move(); };
            _token_store += _mouse.mouse_down += [&](input::Mouse::Button) { process_mouse_down(); };
            _token_store += _mouse.mouse_up += [&](auto) { process_mouse_up(); };
            _token_store += _mouse.mouse_wheel += [&](int16_t delta) { process_mouse_scroll(delta); };
            _token_store += _keyboard.on_key_down += [&](auto key, bool control) { process_key_down(key, control); };
            _token_store += _keyboard.on_char += [&](auto key) { process_char(key); };
        }

        void Input::register_focus_controls(Control* control)
        {
            control->set_input_query(this);

            _token_store += control->on_focus_requested += [this, control]() { set_focus_control(control); };
            _token_store += control->on_focus_clear_requested += [&]() { set_focus_control(nullptr); };
            _token_store += control->on_hierarchy_changed += [this]() 
            {
                register_events();
            };
            _token_store += control->on_deleting += [this, control]()
            {
                if (_focus_control == control)
                {
                    _focus_control = nullptr;
                }
            };

            for (auto& child : control->child_elements())
            {
                register_focus_controls(child);
            }
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
                if (result && result->handles_hover())
                {
                    return result;
                }
            }

            if (control->handles_hover())
            {
                return control;
            }

            return nullptr;
        }

        void Input::process_mouse_down()
        {
            auto position = client_cursor_position(_window);
            process_mouse_down(&_control, position);
        }

        bool Input::process_mouse_down(Control* control, const Point& position)
        {
            if (!control->visible() || !in_bounds(position, control->size()))
            {
                return false;
            }

            for (auto& child : control->child_elements())
            {
                // Convert the position into the coordinate space of the child element.
                if (process_mouse_down(child, position - child->position()))
                {
                    return true;
                }
            }

            // Promote controls to focus control, or clear if there are no controls that 
            // accepted the event.
            bool handled_by_self = control->handles_input() && control->mouse_down(position);
            if (handled_by_self)
            {
                set_focus_control(control);
            }
            else if (!control->parent())
            {
                set_focus_control(nullptr);
            }
            return handled_by_self;
        }

        void Input::process_mouse_up()
        {
            auto position = client_cursor_position(_window);

            if (_focus_control)
            {
                const auto focus = _focus_control;
                const auto control_space_position = position - focus->absolute_position();
                bool focus_handled = focus->mouse_up(control_space_position);
                if (focus_handled && in_bounds(control_space_position, focus->size()))
                {
                    focus->clicked(control_space_position);
                }

                if (focus_handled)
                {
                    return;
                }
            }

            process_mouse_up(&_control, position);
        }

        bool Input::process_mouse_up(Control* control, const Point& position)
        {
            // Bounds check - before child elements are checked.
            if (!control->visible() || !in_bounds(position, control->size()))
            {
                return false;
            }

            for (auto& child : control->child_elements())
            {
                // Convert the position into the coordinate space of the child element.
                if (process_mouse_up(child, position - child->position()))
                {
                    return true;
                }
            }

            // If none of the child elements have handled this event themselves, call the up of the control.
            return control->mouse_up(position);
        }

        void Input::process_mouse_scroll(int16_t delta)
        {
            auto position = client_cursor_position(_window);
            if (_focus_control &&
                _focus_control->visible() &&
                in_bounds(position - _focus_control->absolute_position(), _focus_control->size()))
            {
                _focus_control->scroll(delta);
                return;
            }
            process_mouse_scroll(&_control, position, delta);
        }

        bool Input::process_mouse_scroll(Control* control, const Point& position, int16_t delta)
        {
            // Bounds check - before child elements are checked.
            if (!control->visible() || !in_bounds(position, control->size()))
            {
                return false;
            }

            for (auto& child : control->child_elements())
            {
                // Convert the position into the coordinate space of the child element.
                if (process_mouse_scroll(child, position - child->position(), delta))
                {
                    return true;
                }
            }

            // If none of the child elements have handled this event themselves, call the 
            // scroll function of this control.
            return control->scroll(delta);
        }

        void Input::process_key_down(uint16_t key, bool)
        {
            if (_focus_control && _focus_control->key_down(key))
            {
                return;
            }
            process_key_down(&_control, key);
        }

        bool Input::process_key_down(Control* control, uint16_t key)
        {
            if (!control->visible())
            {
                return false;
            }

            for (auto& child : control->child_elements())
            {
                if (process_key_down(child, key))
                {
                    return true;
                }
            }

            // If none of the child elements have handled this event themselves, call the key_down
            // event of the control.
            return control->key_down(key);
        }

        void Input::process_char(uint16_t key)
        {
            if (_focus_control && _focus_control->key_char(key))
            {
                return;
            }
            process_char(&_control, key);
        }

        bool Input::process_char(Control* control, uint16_t key)
        {
            if (!control->visible())
            {
                return false;
            }

            for (auto& child : control->child_elements())
            {
                if (process_char(child, key))
                {
                    return true;
                }
            }
            return control->key_char(key);
        }

        void Input::set_focus_control(Control* control)
        {
            if (_focus_control && _focus_control != control)
            {
                _focus_control->lost_focus(control);
            }
            _focus_control = control;
            if (_focus_control)
            {
                _focus_control->gained_focus();
            }
        }
    }
}