#pragma once

#include "../IRenderTarget.h"

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            struct MockRenderTarget : public IRenderTarget
            {
                MockRenderTarget();
                virtual ~MockRenderTarget();
                MOCK_METHOD(void, clear, (const DirectX::SimpleMath::Color& colour), (override));
                MOCK_METHOD(void, apply, (), (override));
                MOCK_METHOD(Texture, texture, (), (const, override));
                MOCK_METHOD(Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, render_target, (), (const, override));
                MOCK_METHOD(uint32_t, width, (), (const, override));
                MOCK_METHOD(uint32_t, height, (), (const, override));
                MOCK_METHOD(Size, size, (), (const, override));
            };
        }
    }
}