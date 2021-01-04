#pragma once

#include <trview.ui/Window.h>

namespace trview
{
    namespace ui
    {
        class Modal final : public Window
        {
        public:
            explicit Modal(const Size& size, const Colour& background_colour);
            virtual ~Modal() = default;
            virtual bool mouse_down(const Point& position) override;
            virtual bool mouse_up(const Point& position) override;
            virtual bool clicked(Point position) override;
        };
    }
}
