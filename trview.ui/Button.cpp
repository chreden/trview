#include "Button.h"

namespace trview
{
    namespace ui
    {
        Button::Button(Point position, Size size, Texture up_image, Texture down_image)
            : Image(position, size), _up_image(up_image), _down_image(down_image)
        {
            set_texture(_up_image);
        }

        bool Button::clicked(Point)
        {
            on_click();
            return true;
        }
    }
}
