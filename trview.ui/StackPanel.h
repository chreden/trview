#pragma once

#include "Window.h"

namespace trview
{
    namespace ui
    {
        class StackPanel final : public Window
        {
        public:
            StackPanel(Point position, Size size, Colour colour, Size padding);
            virtual void add_child(std::unique_ptr<Control>&& child_element) override;
            virtual ~StackPanel() = default;
        private:
            Point get_next_position() const;
            const Size _padding;
        };
    }
}
