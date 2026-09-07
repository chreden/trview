export module trview.graphics:SpriteSizeStore;

import std.compat;
import trview.common;
import :ISprite;

namespace trview
{
    namespace graphics
    {
        // Used to set and restore the host size of a sprite - useful when using the 
        // sprite to render to a different render target.
        export class SpriteSizeStore final
        {
        public:
            // Begin a sprite size store operation, setting the sprite to the new size specified
            // after storing the old host size for later restoration.
            // sprite: The sprite to operate on.
            // new_size: The new host size of the sprite.
            explicit SpriteSizeStore(ISprite& sprite, const Size& new_size);

            // When this executes it will restore the host size of the sprite to what it was when
            // the SpriteSizeStore was created.
            ~SpriteSizeStore();
        private:
            ISprite& _sprite;
            Size _size;
        };
    }
}
