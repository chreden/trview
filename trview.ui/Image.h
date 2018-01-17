#pragma once

#include <trview.common/Texture.h>
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

            void set_texture(Texture texture);

            Texture texture() const;
        private:
            Texture _texture;
        };
    }
}
