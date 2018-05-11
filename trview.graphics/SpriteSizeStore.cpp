#include "SpriteSizeStore.h"
#include "Sprite.h"

namespace trview
{
    namespace graphics
    {
        SpriteSizeStore::SpriteSizeStore(Sprite& sprite)
            : _sprite(sprite), _width(sprite.host_width()), _height(sprite.host_height())
        {
        }

        SpriteSizeStore::~SpriteSizeStore()
        {
            _sprite.set_host_size(_width, _height);
        }
    }
}
