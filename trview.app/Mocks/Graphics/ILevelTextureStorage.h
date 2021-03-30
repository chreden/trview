#pragma once

#include <trview.app/Graphics/ILevelTextureStorage.h>

namespace trview
{
    class MockLevelTextureStorage final : public ILevelTextureStorage
    {
        MOCK_METHOD(graphics::Texture, coloured, (uint32_t), (const));
        MOCK_METHOD(graphics::Texture, lookup, (const std::string&), (const));
        MOCK_METHOD(void, store, (const std::string&, const graphics::Texture&));
        MOCK_METHOD(graphics::Texture, texture, (uint32_t texture_index), (const));
        MOCK_METHOD(graphics::Texture, untextured, (), (const));
        MOCK_METHOD(DirectX::SimpleMath::Vector2, uv, (uint32_t, uint32_t), (const));
        MOCK_METHOD(uint32_t, tile, (uint32_t), (const));
        MOCK_METHOD(uint32_t, num_tiles, (), (const));
        MOCK_METHOD(uint16_t, attribute, (uint32_t), (const));
        MOCK_METHOD(DirectX::SimpleMath::Color, palette_from_texture, (uint32_t), (const));
    };
}
