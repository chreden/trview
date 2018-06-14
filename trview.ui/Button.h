#pragma once

#include <trview.graphics/Texture.h>
#include <trview.common/Event.h>

#include "Control.h"

namespace trview
{
    namespace ui
    {
        class Button : public Control
        {
        public:
            Button(Point position, Size size, graphics::Texture up_image, graphics::Texture down_image);
            virtual ~Button() = default;

            // This event is raised when the user clicks on the button.
            Event<> on_click;
        protected:
            virtual bool clicked(Point position) override;
        private:
            graphics::Texture _up_image;
            graphics::Texture _down_image;
        };
    }
}
