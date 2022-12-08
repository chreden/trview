#pragma once

#include "../../Graphics/ILevelTextureStorage.h"

namespace trview
{
    namespace mocks
    {
        struct MockLevelTextureStorage : public ILevelTextureStorage
        {
            MockLevelTextureStorage();
            virtual ~MockLevelTextureStorage();
            MOCK_METHOD(graphics::Texture, coloured, (uint32_t), (const, override));
            MOCK_METHOD(graphics::Texture, lookup, (const std::string&), (const, override));
            MOCK_METHOD(void, store, (const std::string&, const graphics::Texture&), (override));
            MOCK_METHOD(graphics::Texture, texture, (uint32_t texture_index), (const, override));
            MOCK_METHOD(graphics::Texture, untextured, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector2, uv, (uint32_t, uint32_t), (const, override));
            MOCK_METHOD(uint32_t, tile, (uint32_t), (const, override));
            MOCK_METHOD(uint32_t, num_tiles, (), (const, override));
            MOCK_METHOD(uint16_t, attribute, (uint32_t), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Color, palette_from_texture, (uint32_t), (const, override));
            MOCK_METHOD(graphics::Texture, geometry_texture, (), (const, override));
        };
    }
}
