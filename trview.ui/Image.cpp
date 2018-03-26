#include "Image.h"

namespace trview
{
    namespace ui
    {
        Image::Image(Point position, Size size)
            : Window(position, size, Colour(0.0f, 0.0f, 0.0f, 0.0f))
        {
        }

        Texture Image::texture() const
        {
            return _texture;
        }

        void Image::set_texture(Texture texture)
        {
            _texture = texture;
        }
    }
}