#pragma once

#include "ITextureStorage.h"

namespace trview
{
    struct ILevelTextureStorage : public ITextureStorage
    {
        virtual ~ILevelTextureStorage() = 0;

        virtual Texture texture(uint32_t texture_index) const = 0;

        virtual DirectX::XMFLOAT2 uv(uint32_t texture_index, uint32_t uv_index) const = 0;

        virtual uint32_t tile(uint32_t texture_index) const = 0;

        virtual uint32_t num_tiles() const = 0;

        virtual DirectX::XMFLOAT4 palette_from_texture(uint32_t texture) const = 0;

        virtual DirectX::XMFLOAT4 palette(uint32_t index) const = 0;
    };
}

