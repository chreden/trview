#pragma once

#include "../IBuffer.h"

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            struct MockBuffer : public IBuffer
            {
                MockBuffer();
                virtual ~MockBuffer();
                MOCK_METHOD(void, apply, (const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, ApplyTo target), (override));
                MOCK_METHOD(void, set_data, (const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const void* const, uint32_t), (override));
            };
        }
    }
}
