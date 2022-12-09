#pragma once

#include "../IDeviceWindow.h"

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            struct MockDeviceWindow : public IDeviceWindow
            {
                MockDeviceWindow();
                virtual ~MockDeviceWindow();
                MOCK_METHOD(void, begin, (), (override));
                MOCK_METHOD(void, clear, (const DirectX::SimpleMath::Color&), (override));
                MOCK_METHOD(void, present, (bool), (override));
                MOCK_METHOD(void, resize, (), (override));
                MOCK_METHOD(Microsoft::WRL::ComPtr<ID3D11DeviceContext>, context, (), (const, override));
            };
        }
    }
}
