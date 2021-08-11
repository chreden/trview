#pragma once

import trview.common.size;
#include "Texture.h"

namespace trview
{
    namespace graphics
    {
        struct ISprite
        {
            using Source = std::function<std::unique_ptr<ISprite>(const Size& size)>;

            virtual ~ISprite() = 0;
            virtual void render(const Texture& texture, float x, float y, float width, float height, DirectX::SimpleMath::Color colour = { 1,1,1,1 }) = 0;
            virtual Size host_size() const = 0;
            virtual void set_host_size(const Size& size) = 0;
        };
    }
}
