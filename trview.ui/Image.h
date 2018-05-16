#pragma once

#include <trview.graphics/Texture.h>
#include "Window.h"

namespace trview
{
    namespace ui
    {
        class Image : public Window
        {
        public:
            explicit Image(Point position, Size size);

            virtual ~Image() = default;

            void set_texture(graphics::Texture texture);

            graphics::Texture texture() const;
        private:
            graphics::Texture _texture;
        };
    }
}
