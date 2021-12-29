/// @file Control.h
/// @brief Base class for user interface windows and controls.

#pragma once

#include <vector>
#include <memory>
#include <trview.common/Event.h>
#include <trview.common/Size.h>
#include <trview.common/Point.h>
#include <trview.common/TokenStore.h>
#include <trview.common/Colour.h>
#include "Align.h"
#include "IInput.h"
#include "Layouts/ILayout.h"
#include "SizeDimension.h"

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
            /// @param check_tree Whether to check up from this control.
            /// @returns Whether the control is visible.
            bool visible(bool check_tree = false) const;

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
            template <typename T>
            T* add_child(std::unique_ptr<T>&& child_element);

            /// Remove all child elements from the control.
            virtual void clear_child_elements();

            /// Get the elements that are direct children of this element.
            /// @param rendering_order If true, this will sort the controls in reverse z order.
            /// @remarks To add a new child, use add_child.
            /// @returns The child elements.
            std::vector<Control*> child_elements(bool rendering_order = false) const;

            /// Determines whether the mouse is over the element or any child elements that are
            /// interested in taking input.
            /// @param position The mouse position.
            /// @returns True if the control or any child elements are under the cursor and the control handles the event.
            bool is_mouse_over(const Point& position) const;

            /// Gets whether this control handles input when tested in mouse over events.
            bool handles_input() const;

            /// Set whether this control handles input when tested in is_mouse_over. Defaults to true.
            /// @param value Whether the control handles input.
            virtual void set_handles_input(bool value);

            /// Gets whether this control handles mouse hover events.
            bool handles_hover() const;

            /// Set whether this control handles mouse hover events. Defaults to false.
            /// @param value Whether the control handles mouse hover events.
            void set_handles_hover(bool value);

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

            /// Find the first control with the specified name and type.
            /// @param name The name to search for.
            /// @returns The control with the given name as the specified type. If there is
            ///          no control found, this returns null.
            template <typename T>
            T* find(const std::string& name);

            /// <summary>
            /// Find all controls with the specified type.
            /// </summary>
            /// <typeparam name="T">The control type to find.</typeparam>
            /// <returns>The controls with the given types. If there are no control found, this returns an empty vector.</returns>
            template <typename T>
            std::vector<const T*> find() const;

            /// <summary>
            /// Find all controls with the specified type.
            /// </summary>
            /// <typeparam name="T">The control type to find.</typeparam>
            /// <returns>The controls with the given types. If there are no control found, this returns an empty vector.</returns>
            template <typename T>
            std::vector<T*> find();

            /// Get the z order of the control.
            /// @returns The z order.
            int z() const;

            /// <summary>
            /// Set the Z order of the control. Lower is on top.
            /// </summary>
            /// <param name="value">The new Z order</param>
            void set_z(int value);

            /// Remove the specified child element.
            /// @param child_element The element to remove.
            std::unique_ptr<Control> remove_child(Control* child_element);

            /// Event raised when a child is added.
            Event<Control*> on_add_child;

            /// <summary>
            /// Event raised when a child is removed.
            /// </summary>
            Event<Control*> on_remove_child;

            /// Event raised when the size of the control has changed.
            Event<Size> on_size_changed;

            /// Event raised when the control has changed and needs to be redrawn.
            Event<> on_invalidate;

            /// Event raised when there has been a change to the children of this control.
            Event<> on_hierarchy_changed;

            /// Event raised when the control wants to become the focus control.
            Event<> on_focus_requested;

            /// Event raised when the control wants to clear the focus.
            Event<> on_focus_clear_requested;

            /// <summary>
            /// Event raised when the control is no longer the focus.
            /// </summary>
            Event<> on_focus_lost;

            /// Event raised when the control is being deleted.
            Event<> on_deleting;

            /// Event raised when user has selected the control for text tinput.
            Event<> on_focused;
            /// <summary>
            /// Event raised when the control has been updated. The elapsed time is passed as the parameter.
            /// </summary>
            Event<float> on_update;

            /// <summary>
            /// Event raised when the mouse has entered the control bounds.
            /// </summary>
            Event<> on_mouse_enter;
            /// <summary>
            /// Event raised when the mouse has moved when over the control.
            /// </summary>
            Event<> on_mouse_move;
            /// <summary>
            /// Event raised when the mouse has left the control bounds.
            /// </summary>
            Event<> on_mouse_leave;

            /// To be called when the mouse has been pressed down over the element.
            /// @param position The position of the mouse down relative to the control.
            /// @return True if the event was handled by the element.
            virtual bool mouse_down(const Point& position);

            /// To be called when the mouse has been reelase over the element.
            /// @param position The position of the mouse up relative to the control.
            /// @return True if the event was handled by the element.
            virtual bool mouse_up(const Point& position);

            /// To be called when the mouse has hovered over the element.
            virtual void mouse_enter();

            /// To be called when the mouse has stopped hovering over the element.
            virtual void mouse_leave();

            /// To be called when the user interface element has been clicked.
            /// This should be overriden by child elements to handle a click.
            /// @param position The position of the click relative to the control.
            virtual bool clicked(Point position);

            /// To be called when the control has become the new focus control.
            virtual void gained_focus();

            /// To be called when the user clicks away from a focus control.
            virtual void lost_focus(Control* new_focus);

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
            virtual bool key_down(uint16_t key, bool control_pressed, bool shift_pressed);

            /// To be called when a key character is pressed.
            /// This should be overriden by child elements to handle a key char event.
            /// @param key The key that was pressed.
            /// @returns True if the key char event was handled.
            virtual bool key_char(wchar_t key);

            virtual bool paste(const std::wstring& text);

            virtual bool copy(std::wstring& output);

            virtual bool cut(std::wstring& output);

            void set_input(IInput* input);

            bool focused() const;

            void set_layout(std::unique_ptr<ILayout> layout);
            /// <summary>
            /// Update the control.
            /// </summary>
            /// <param name="delta">The elapsed time since the previous update.</param>
            void update(float delta);

            const ILayout* const layout() const;

            void set_auto_size_dimension(SizeDimension dimension);
        protected:
            /// To be called after a child element has been added to the control.
            /// @param child_element The element that was added.
            virtual void inner_add_child(Control* child_element);

            TokenStore _token_store;
            IInput* _input{ nullptr };
        private:
            void set_parent(Control* parent);

            std::vector<std::unique_ptr<Control>> _child_elements;

            Control* _parent{ nullptr };
            Point    _position;
            Size     _size;
            bool     _visible;
            bool     _handles_input{ true };
            bool     _handles_hover{ false };
            Align    _horizontal_alignment{ Align::Near };
            Align    _vertical_alignment{ Align::Near };
            std::string _name;
            int      _z{ 0 };
            bool     _focused{ false };
            std::unique_ptr<ILayout> _layout;
            SizeDimension _auto_size_dimension{ SizeDimension::None };
        };
    }
}

#include "Control.inl"
