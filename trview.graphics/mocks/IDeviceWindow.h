#pragma once

#include "../IDeviceWindow.h"

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            class MockDeviceWindow final : public IDeviceWindow
            {
            public:
                virtual ~MockDeviceWindow() = default;
                MOCK_METHOD(void, begin, ());
                MOCK_METHOD(void, clear, (const DirectX::SimpleMath::Color&));
                MOCK_METHOD(void, present, (bool));
                MOCK_METHOD(void, resize, ());
                MOCK_METHOD(Microsoft::WRL::ComPtr<ID3D11DeviceContext>, context, (), (const));
            };
        }
    }
}
