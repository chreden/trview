#pragma once

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            struct MockShader : public IShader
            {
                virtual ~MockShader() = default;
                MOCK_METHOD(void, apply, (const Microsoft::WRL::ComPtr<ID3D11DeviceContext>&), (override));
            };
        }
    }
}

