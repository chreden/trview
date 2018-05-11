#pragma once

#include <cstdint>

namespace trview
{
    namespace graphics
    {
        class Sprite;

        // Used to set and restore the host size of a sprite - useful when using the 
        // sprite to render to a different render target.
        class SpriteSizeStore final
        {
        public:
            // Begin a sprite size store operation, setting the sprite to the new size specified
            // after storing the old host size for later restoration.
            // sprite: The sprite to operate on.
            // new_width: The new host width of the sprite.
            // new_height: The new host height of the sprite.
            explicit SpriteSizeStore(Sprite& sprite, uint32_t new_width, uint32_t new_height);

            // When this executes it will restore the host size of the sprite to what it was when
            // the SpriteSizeStore was created.
            ~SpriteSizeStore();
        private:
            Sprite& _sprite;
            uint32_t _width;
            uint32_t _height;
        };
    }
}
