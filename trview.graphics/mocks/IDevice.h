#pragma once

#include "../IDevice.h"

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            class MockDevice final : public IDevice
            {
            public:
                virtual ~MockDevice() = default;
                MOCK_METHOD(void, begin, ());
                MOCK_METHOD(const Microsoft::WRL::ComPtr<ID3D11Device>&, device, (), (const));
                MOCK_METHOD(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>&, context, (), (const));
                MOCK_METHOD(std::unique_ptr<DeviceWindow>, create_for_window, (const Window&));
            };
        }
    }
}