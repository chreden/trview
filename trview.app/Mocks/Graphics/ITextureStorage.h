#pragma once

#include "../../Graphics/ITextureStorage.h"

namespace trview
{
    namespace mocks
    {
        struct MockTextureStorage : public ITextureStorage
        {
            MockTextureStorage();
            virtual ~MockTextureStorage();
            MOCK_METHOD(void, add_texture, (const std::vector<uint32_t>&, uint32_t, uint32_t), (override));
            MOCK_METHOD(graphics::Texture, coloured, (uint32_t), (const, override));
            MOCK_METHOD(graphics::Texture, geometry_texture, (), (const, override));
            MOCK_METHOD(graphics::Texture, lookup, (const std::string&), (const, override));
            MOCK_METHOD(uint32_t, num_textures, (), (const, override));
            MOCK_METHOD(void, store, (const std::string&, const graphics::Texture&), (override));
            MOCK_METHOD(graphics::Texture, texture, (uint32_t), (const, override));
            MOCK_METHOD(graphics::Texture, untextured, (), (const, override));
        };
    }
}
