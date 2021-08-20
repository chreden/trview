#pragma once

#include "ILayout.h"
#include <trview.common/Size.h>
#include <trview.common/Point.h>
#include <trview.common/TokenStore.h>
#include "../SizeMode.h"
#include "../SizeDimension.h"

namespace trview
{
    namespace ui
    {
        class StackLayout final : public ILayout
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

            StackLayout(Size padding = Size(), Direction direction = Direction::Vertical, SizeMode size_mode = SizeMode::Auto);
            virtual ~StackLayout() = default;
            virtual void bind(Control& control) override;
            /// Set which dimensions in which the stack panel will automatically expand.
            /// @param dimension The dimension in which to expand.
            void set_auto_size_dimension(SizeDimension dimension);
            /// Set the margin value to space before the first and after the last elements.
            /// @param margin The margin to apply.
            void set_margin(const Size& margin);
        private:
            Point get_next_position() const;
            Point get_next_position(Point previous_position, Size previous_size) const;
            void recalculate_layout();
            Point get_padding() const;

            TokenStore _token_store;
            const Size _padding;
            Size _margin;
            const Direction _direction;
            SizeMode _size_mode;
            SizeDimension _size_dimension{ SizeDimension::All };
            Control* _control{ nullptr };
        };
    }
}