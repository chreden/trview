#include "SpriteSizeStore.h"
#include "Sprite.h"

namespace trview
{
    namespace graphics
    {
        // Begin a sprite size store operation, setting the sprite to the new size specified
        // after storing the old host size for later restoration.
        // sprite: The sprite to operate on.
        // new_width: The new host width of the sprite.
        // new_height: The new host height of the sprite.
        SpriteSizeStore::SpriteSizeStore(Sprite& sprite, uint32_t new_width, uint32_t new_height)
            : _sprite(sprite), _width(sprite.host_width()), _height(sprite.host_height())
        {
            _sprite.set_host_size(new_width, new_height);
        }

        // When this executes it will restore the host size of the sprite to what it was when
        // the SpriteSizeStore was created.
        SpriteSizeStore::~SpriteSizeStore()
        {
            _sprite.set_host_size(_width, _height);
        }
    }
}
