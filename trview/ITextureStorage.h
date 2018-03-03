#pragma once

#include <cstdint>
#include <DirectXMath.h>
#include <trview.common/Texture.h>

namespace trview
{
    struct ITextureStorage
    {
        virtual ~ITextureStorage() = 0;
        virtual Texture coloured(uint32_t colour) const = 0;
    };
}
