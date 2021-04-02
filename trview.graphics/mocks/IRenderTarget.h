#pragma once

#include "../IRenderTarget.h"

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            class MockRenderTarget final : public IRenderTarget
            {
                MOCK_METHOD(void, clear, (const DirectX::SimpleMath::Color& colour));
                MOCK_METHOD(void, apply, ());
                MOCK_METHOD(const Texture&, texture, (), (const));
                MOCK_METHOD(Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, render_target, (), (const));
                MOCK_METHOD(uint32_t, width, (), (const));
                MOCK_METHOD(uint32_t, height, (), (const));
                MOCK_METHOD(Size, size, (), (const));
            };
        }
    }
}