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
        /// <summary>
        /// Layout that places elements in a horiztonal or vertical line.
        /// </summary>
        class StackLayout final : public ILayout
        {
        public:
            /// <summary>
            /// The direction in which to lay elements out.
            /// </summary>
            enum class Direction
            {
                /// <summary>
                /// Place elements below one another.
                /// </summary>
                Vertical,
                /// <summary>
                /// Place elements side by side.
                /// </summary>
                Horizontal
            };

            StackLayout(float padding = 0.0f, Direction direction = Direction::Vertical, SizeMode size_mode = SizeMode::Auto);
            virtual ~StackLayout() = default;
            virtual void bind(Control& control) override;
            /// <summary>
            /// Set which dimensions in which the stack panel will automatically expand.
            /// </summary>
            /// <param name="dimension">The dimension in which to expand.</param>
            void set_auto_size_dimension(SizeDimension dimension);
            /// <summary>
            /// Set the margin value to space before the first and after the last elements.
            /// </summary>
            /// <param name="margin">The margin to apply.</param>
            void set_margin(const Size& margin);

            SizeMode size_mode() const;
            float padding() const;
            Direction direction() const;
            Size margin() const;
            SizeDimension size_dimension() const;
        private:
            Point get_next_position() const;
            Point get_next_position(Point previous_position, Size previous_size) const;
            void recalculate_layout();
            Point get_padding() const;

            TokenStore _token_store;
            const float _padding;
            Size _margin;
            const Direction _direction;
            SizeMode _size_mode;
            SizeDimension _size_dimension{ SizeDimension::All };
            Control* _control{ nullptr };
        };
    }
}