/// @file StackPanel.h
/// @brief UI element that lays out elements either vertically or horizontally.

#pragma once

#include "Window.h"
#include "SizeMode.h"
#include "SizeDimension.h"
#include <trview.common/TokenStore.h>

namespace trview
{
    namespace ui
    {
        /// UI element that lays out elements either vertically or horizontally.
        class StackPanel : public Window
        {
        public:
            /// The direction in which to lay elements out.
            enum class Direction
            {
                /// Place elements below one another.
                Vertical,
                /// Place elements side by side.
                Horizontal
            };

            /// Creates a new StackPanel instance.
            /// @param position The position of the stack panel inside the parent control.
            /// @param size The initial size of the stack panel.
            /// @param colour The background colour of the stack panel.
            /// @param padding The amount of padding to place between each element.
            /// @param direction The direction in which to stack elements. Defaults to Vertical.
            /// @param size_mode Whether or not to resize the panel automatically.
            StackPanel(Point position, Size size, Colour colour, Size padding, Direction direction = Direction::Vertical, SizeMode size_mode = SizeMode::Auto);

            /// Add a child to this element. This will place the new element in the appropriate
            /// position based on the layout direction and the elements already in the control.
            /// @param child_element The element to add as a child.
            virtual void add_child(std::unique_ptr<Control>&& child_element) override;

            /// Destructor for StackPanel.
            virtual ~StackPanel() = default;

            /// Set which dimensions in which the stack panel will automatically expand.
            /// @param dimension The dimension in which to expand.
            void set_auto_size_dimension(SizeDimension dimension);
        private:
            Point get_next_position() const;
            Point get_next_position(Point previous_position, Size previous_size) const;
            void recalculate_layout();
            const Size _padding;
            const Direction _direction;
            SizeMode _size_mode;
            SizeDimension _size_dimension{ SizeDimension::All };
            TokenStore _token_store;
        };
    }
}
