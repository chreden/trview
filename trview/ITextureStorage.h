#pragma once

#include <cstdint>
#include <DirectXMath.h>
#include <trview.common/Texture.h>

namespace trview
{
    struct ITextureStorage
    {
        virtual ~ITextureStorage() = 0;

        virtual Texture texture(uint32_t texture_index) const = 0;

        virtual DirectX::XMFLOAT2 uv(uint32_t texture_index, uint32_t uv_index) const = 0;

        virtual uint32_t tile(uint32_t texture_index) const = 0;
    };
}
