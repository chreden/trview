#pragma once

#include "../ISprite.h"

namespace trview
{
    namespace graphics
    {
        class MockSprite final : public ISprite
        {
            MOCK_METHOD(void, render, (const Microsoft::WRL::ComPtr<ID3D11DeviceContext>&, const Texture&, float, float, float, float, DirectX::SimpleMath::Color));
            MOCK_METHOD(Size, host_size, (), (const));
            MOCK_METHOD(void, set_host_size, (const Size&));
        };
    }
}