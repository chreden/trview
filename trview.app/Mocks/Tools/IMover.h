#pragma once

#include <trview.app/Tools/IMover.h>

namespace trview
{
    namespace mocks
    {
        struct MockMover final : public IMover
        {
            virtual ~MockMover() = default;
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&), (override));
            MOCK_METHOD(void, set_position, (const DirectX::SimpleMath::Vector3&), (override));
        };
    }
}