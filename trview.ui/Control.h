#pragma once

#include <vector>
#include <memory>
#include <trview.common/Event.h>
#include <trview.common/Size.h>

#include "Point.h"

namespace trview
{
    namespace ui
    {
        // Control is the base class for all user interface windows and controls.
        class Control
        {
        public:
            Control(Point position, Size size);

            virtual ~Control() = 0;

            // Get the X and Y position of the control relative to the parent window.
            // To change the position of the control, call set_position.
            Point position() const;

            // Set the X and Y position of the control relative to the parent window.
            // To get the position of the control, call position.
            void set_position(Point position);

            // Get the width and height of the control.
            Size size() const;

            // Gets whether the control and its child controls are visible. Invisible
            // controls will not be rendered and will not accept input.
            // To set the visibility state of a control, use set_visible(bool);
            bool visible() const;

            // Sets whether the control and its child controls are visible. Invisible
            // controls will not be rendered and will not accept input.
            // To query the current state, use visible().
            void set_visible(bool value);

            // Get the parent control of this control. This is a non-owning pointer
            // to the parent and can be null in the case of this being a top-level
            // control.
            Control* parent() const;

            // Add the specified control as a child element of this control.
            // Sets the parent element of the child to be this element.
            virtual void add_child(std::unique_ptr<Control>&& child_element);

            // Get the elements that are direct children of this element.
            // To add a new child, use add_child.
            std::vector<Control*> child_elements() const;

            // Process a mouse_down event at the position specified.
            // Returns whether the mouse click was within the bounds of the control element and was handled.
            bool mouse_down(Point position);

            bool mouse_up(Point position);

            // Process a mouse_move event at the position specified.
            // Returns whether the mouse move was handled.
            bool mouse_move(Point position);

            // Determines whether the mouse is over the element or any child elements that are interested
            // in taking input.
            // position: The mouse position.
            // Returns: True if the control or any child elements are under the cursor.
            bool is_mouse_over(Point position) const;

            // Set whether this control handles input when tested in is_mouse_over. Defaults to true.
            // value: Whether the control handles input.
            void set_handles_input(bool value);

            // Set the size of the control.
            // size: The new size of the control.
            void set_size(Size size);

            // Event raised when the size of the control has changed.
            Event<Size> on_size_changed;

            // The control has changed and needs to be redrawn.
            Event<void> on_invalidate;
        protected:
            // To be called when the user interface element has been clicked.
            // This should be overriden by child elements to handle a click.
            virtual bool clicked(Point position);

            // To be called when the mouse was moved over the element.
            // This should be overriden by child elements to handle a move.
            virtual bool move(Point position);

            void set_focus_control(Control* control);

            Control* focus_control() const;
        private:
            std::vector<std::unique_ptr<Control>> _child_elements;
            Control* _parent{ nullptr };
            Control* _focus_control{ nullptr };
            Point    _position;
            Size     _size;
            bool     _visible;
            bool     _handles_input{ true };
        };
    }
}
