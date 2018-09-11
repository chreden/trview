/// @file Control.h
/// @brief Base class for user interface windows and controls.

#pragma once

#include <vector>
#include <memory>
#include <trview.common/Event.h>
#include <trview.common/Size.h>
#include <trview.common/Point.h>
#include "Align.h"

namespace trview
{
    namespace ui
    {
        /// Control is the base class for all user interface windows and controls.
        class Control
        {
        public:
            /// Create a new control.
            /// @param position The position in the parent control.
            /// @param size The size of the control.
            Control(Point position, Size size);

            /// Destructor for control.
            virtual ~Control() = 0;

            /// Get the horizontal alignment value. This will affect how the control is placed in the parent control.
            /// @returns The horizontal alignment mode.
            Align horizontal_alignment() const;

            /// Get the vertical alignment value. This will affect how the control is placed in the parent control.
            /// @returns The vertical alignment mode.
            Align vertical_alignment() const;

            /// Set the vertical alignment value. This will affect how the control is placed in the parent control.
            /// @param mode The vertical alignment mode.
            void set_vertical_alignment(Align mode);

            /// Set the horizontal alignment value. This will affect how the control is placed in the parent control.
            /// @param mode The horizontal alignment mode.
            void set_horizontal_alignment(Align mode);

            /// Get the X and Y position of the control relative to the parent window.
            /// To change the position of the control, call set_position.
            /// @returns The position of the control relative to the parent.
            Point position() const;

            /// Get the absolute position of the control relative the to most distant parent.
            /// @returns The absolute position of the control.
            Point absolute_position() const;

            /// Set the X and Y position of the control relative to the parent window.
            /// To get the position of the control, call position.
            /// @param position The position of the control in the parent window.
            void set_position(Point position);

            /// Get the width and height of the control.
            /// @returns The size of the control.
            Size size() const;

            /// Gets whether the control and its child controls are visible. Invisible
            /// controls will not be rendered and will not accept input.
            /// To set the visibility state of a control, use set_visible(bool);
            /// @returns Whether the control is visible.
            bool visible() const;

            /// Sets whether the control and its child controls are visible. Invisible
            /// controls will not be rendered and will not accept input.
            /// @remarks To query the current state, use visible().
            /// @param value Whether the control is visible.
            void set_visible(bool value);

            /// Get the parent control of this control. This is a non-owning pointer
            /// to the parent and can be null in the case of this being a top-level control.
            /// @returns The parent control.
            Control* parent() const;

            /// Add the specified control as a child element of this control.
            /// Sets the parent element of the child to be this element.
            /// @param child_element The element to add to the control.
            virtual void add_child(std::unique_ptr<Control>&& child_element);

            /// Remove all child elements from the control.
            virtual void clear_child_elements();

            /// Get the elements that are direct children of this element.
            /// @remarks To add a new child, use add_child.
            /// @returns The child elements.
            std::vector<Control*> child_elements() const;

            /// Process a mouse_down event at the position specified.
            /// @param position The position of the mouse relative to the control.
            /// @returns Whether the mouse down event was handled by the control.
            bool process_mouse_down(const Point& position);

            /// Process a mouse_up event a tthe position specified.
            /// @param position The position of the mouse relative to the control.
            /// @returns Whether the mouse up event was handled by the control.
            bool process_mouse_up(const Point& position);

            /// Process a mouse_move event at the position specified.
            /// @param position The position of the mouse relative to the control.
            /// @returns Whether the mouse move was handled by the control.
            bool process_mouse_move(const Point& position);

            /// Process a mouse_scroll event.
            /// @param position The position of the mouse relative to the control.
            /// @param delta The mouse wheel movement.
            /// @returns Whether the mouse scroll was handled by the control.
            bool mouse_scroll(const Point& position, int16_t delta);

            /// Determines whether the mouse is over the element or any child elements that are
            /// interested in taking input.
            /// @param position The mouse position.
            /// @returns True if the control or any child elements are under the cursor and the control handles the event.
            bool is_mouse_over(const Point& position) const;

            /// Set whether this control handles input when tested in is_mouse_over. Defaults to true.
            /// @param value Whether the control handles input.
            void set_handles_input(bool value);

            /// Process a key down event.
            /// @param key The key that was pressed down.
            /// @returns True if the event was processed by the control.
            bool process_key_down(uint16_t key);

            /// Set the size of the control.
            /// @param size The new size of the control.
            void set_size(Size size);

            /// Gets the name of the control.
            /// @returs The name of the control.
            const std::string& name() const;

            /// Set the new name of the control.
            /// @param name The new name of the control.
            void set_name(const std::string& name);

            /// Find the first control with the specified name and type.
            /// @param name The name to search for.
            /// @returns The control with the given name as the specified type. If there is
            ///          no control found, this returns null.
            template <typename T>
            T* find(const std::string& name) const;

            /// Event raised when the size of the control has changed.
            Event<Size> on_size_changed;

            /// Event raised when the control has changed and needs to be redrawn.
            Event<> on_invalidate;

            /// Event raised when there has been a change to the children of this control.
            Event<> on_hierarchy_changed;
        protected:
            /// To be called when the mouse has been pressed down over the element.
            /// @param position The position of the mouse down relative to the control.
            /// @return True if the event was handled by the element.
            virtual bool mouse_down(const Point& position);

            /// To be called when the mouse has been reelase over the element.
            /// @param position The position of the mouse up relative to the control.
            /// @return True if the event was handled by the element.
            virtual bool mouse_up(const Point& position);

            /// To be called when the user interface element has been clicked.
            /// This should be overriden by child elements to handle a click.
            /// @param position The position of the click relative to the control.
            virtual bool clicked(Point position);

            /// To be called when the mouse was moved over the element.
            /// This should be overriden by child elements to handle a move.
            /// @param position The mouse position relative to the control.
            virtual bool move(Point position);

            /// To be called when the mouse was scrolled over the element.
            /// This should be overiden by child elements to handle a scroll.
            /// @param delta The mouse scroll delta.
            virtual bool scroll(int delta);

            /// To be called when a key was pressed.
            /// This should be overidden by child elements to handle a key down event.
            /// @param key The key that was pressed.
            /// @returns True if the key event was handled.
            virtual bool key_down(uint16_t key);

            /// Set the control in the tree that has focus.
            /// @param control The current focus control
            void set_focus_control(Control* control);

            /// Get the currently focused control.
            /// @returns The currently focused control.
            Control* focus_control() const;
        private:
            /// Set the focus control and recurse to child controls.
            /// @param control The new focus control.
            void inner_set_focus_control(Control* control);

            /// Process a mouse move and recurse to child controls.
            /// @param position The position of the mouse relative to the control.
            bool inner_process_mouse_move(const Point& position);

            /// Process a mouse up and recurse to child controls.
            /// @param position The position of the mouse relative to the control.
            bool inner_process_mouse_up(const Point& position);

            /// Process a key down and recurse to child controls.
            /// @param key The key that was pressed.
            bool inner_process_key_down(uint16_t key);

            std::vector<std::unique_ptr<Control>> _child_elements;

            Control* _parent{ nullptr };
            Control* _focus_control{ nullptr };
            Point    _position;
            Size     _size;
            bool     _visible;
            bool     _handles_input{ true };
            Align    _horizontal_alignment{ Align::Near };
            Align    _vertical_alignment{ Align::Near };
            std::string _name;
        };
    }
}
