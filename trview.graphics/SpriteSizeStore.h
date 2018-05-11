#pragma once

#include <cstdint>

namespace trview
{
    namespace graphics
    {
        class Sprite;

        class SpriteSizeStore final
        {
        public:
            explicit SpriteSizeStore(Sprite& sprite);
            ~SpriteSizeStore();
        private:
            Sprite& _sprite;
            uint32_t _width;
            uint32_t _height;
        };
    }
}
