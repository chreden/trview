#pragma once

#include "../IShader.h"

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            struct MockShader : public IShader
            {
                MockShader();
                virtual ~MockShader();
                MOCK_METHOD(void, apply, (const Microsoft::WRL::ComPtr<ID3D11DeviceContext>&), (override));
            };
        }
    }
}

