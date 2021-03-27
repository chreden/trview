#include "SpriteSizeStore.h"
#include "Sprite.h"

namespace trview
{
    namespace graphics
    {
        // Begin a sprite size store operation, setting the sprite to the new size specified
        // after storing the old host size for later restoration.
        // sprite: The sprite to operate on.
        // new_size: The new host size of the sprite.
        SpriteSizeStore::SpriteSizeStore(ISprite& sprite, const Size& new_size)
            : _sprite(sprite), _size(sprite.host_size())
        {
            _sprite.set_host_size(new_size);
        }

        // When this executes it will restore the host size of the sprite to what it was when
        // the SpriteSizeStore was created.
        SpriteSizeStore::~SpriteSizeStore()
        {
            _sprite.set_host_size(_size);
        }
    }
}
