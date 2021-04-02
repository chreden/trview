#pragma once

#include <cstdint>
#include <trview.app/Graphics/ITextureStorage.h>

namespace trview
{
    namespace mocks
    {
        class MockTextureStorage final : public ITextureStorage
        {
        public:
            virtual ~MockTextureStorage() = default;
            MOCK_METHOD(graphics::Texture, coloured, (uint32_t), (const));
            MOCK_METHOD(graphics::Texture, lookup, (const std::string&), (const));
            MOCK_METHOD(void, store, (const std::string&, const graphics::Texture&));
        };
    }
}
