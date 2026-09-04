module;

#include <d3d11.h>
#include <wrl/client.h>
#include <gmock/gmock.h>

export module trview.graphics:MockShader;

import :IShader;
import :Printers;

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            export struct MockShader : public IShader
            {
                MOCK_METHOD(void, apply, (const Microsoft::WRL::ComPtr<ID3D11DeviceContext>&), (override));
            };
        }
    }
}

