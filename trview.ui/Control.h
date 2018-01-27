#pragma once

#include <vector>
#include <memory>

#include "Point.h"
#include "Size.h"

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
            Point position() const;

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
            void add_child(std::unique_ptr<Control>&& child_element);

            // Get the elements that are direct children of this element.
            // To add a new child, use add_child.
            std::vector<Control*> child_elements() const;

            // Process a mouse_down event at the position specified.
            // Returns whether the mouse click was within the bounds of the control element.
            bool mouse_down(Point position);
        protected:
            // To be called when the user interface element has been clicked.
            // This should be overriden by child elements to handle a click.
            virtual void clicked();
        private:
            std::vector<std::unique_ptr<Control>> _child_elements;
            Control* _parent;
            Point    _position;
            Size     _size;
            bool     _visible;
        };
    }
}
