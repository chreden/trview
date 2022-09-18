#pragma once

#include "../../Elements/CameraSink/ICameraSink.h"

namespace trview
{
    namespace mocks
    {
        struct MockCameraSink : public ICameraSink
        {
            virtual ~MockCameraSink() = default;
            MOCK_METHOD(uint16_t, flag, (), (const, override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(uint32_t, number, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(uint16_t, room, (), (const, override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(bool, visible, (), (const, override));
        };
    }
}
