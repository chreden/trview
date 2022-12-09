#pragma once

#include "../ISprite.h"

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            struct MockSprite : public ISprite
            {
                MockSprite();
                virtual ~MockSprite();
                MOCK_METHOD(void, render, (const Texture&, float, float, float, float, DirectX::SimpleMath::Color), (override));
                MOCK_METHOD(Size, host_size, (), (const, override));
                MOCK_METHOD(void, set_host_size, (const Size&), (override));
            };
        }
    }
}