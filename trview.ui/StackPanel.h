#pragma once

#include "Window.h"
#include "SizeMode.h"

namespace trview
{
    namespace ui
    {
        class StackPanel final : public Window
        {
        public:
            enum class Direction
            {
                Vertical,
                Horizontal
            };

            StackPanel(Point position, Size size, Colour colour, Size padding, Direction direction = Direction::Vertical, SizeMode size_mode = SizeMode::Auto);
            virtual void add_child(std::unique_ptr<Control>&& child_element) override;
            virtual ~StackPanel() = default;
        private:
            Point get_next_position() const;
            Point get_next_position(Point previous_position, Size previous_size) const;
            void recalculate_layout();
            const Size _padding;
            const Direction _direction;
            SizeMode _size_mode;
        };
    }
}
