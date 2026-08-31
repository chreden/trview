module;

#include <gmock/gmock.h>
#include <d3d11.h>

export module trview.graphics:MockBuffer;

import :IBuffer;
import :Printers;

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            export struct MockBuffer : public IBuffer
            {
                MockBuffer();
                virtual ~MockBuffer();
                MOCK_METHOD(void, apply, (const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, ApplyTo target), (override));
                MOCK_METHOD(void, set_data, (const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const void* const, uint32_t), (override));
            };
        }
    }
}
