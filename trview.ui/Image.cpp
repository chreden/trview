#include "Image.h"

namespace trview
{
    namespace ui
    {
        Image::Image(Point position, Size size)
            : Window(position, size, Colour(0.0f, 0.0f, 0.0f, 0.0f))
        {
        }

        graphics::Texture Image::texture() const
        {
            return _texture;
        }

        void Image::set_texture(graphics::Texture texture)
        {
            _texture = texture;
            on_invalidate();
        }
    }
}