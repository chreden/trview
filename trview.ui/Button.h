#pragma once

#include <trview.common/Texture.h>
#include <trview.common/Event.h>

#include "Image.h"

namespace trview
{
    namespace ui
    {
        class Button : public Image
        {
        public:
            Button(Point position, Size size, Texture up_image, Texture down_image);
            virtual ~Button() = default;

            // This event is raised when the user clicks on the button.
            Event<void> on_click;

            void set_state(bool state);
        protected:
            virtual bool clicked(Point position) override;
        private:
            Texture _up_image;
            Texture _down_image;
            bool    _state{ false };
        };
    }
}
