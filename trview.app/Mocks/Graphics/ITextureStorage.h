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
            MOCK_METHOD(graphics::Texture, coloured, (uint32_t), (const, override));
            MOCK_METHOD(graphics::Texture, lookup, (const std::string&), (const, override));
            MOCK_METHOD(void, store, (const std::string&, const graphics::Texture&), (override));
        };
    }
}
