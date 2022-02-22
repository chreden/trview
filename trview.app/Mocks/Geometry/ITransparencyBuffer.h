#pragma once

#include "../../Geometry/ITransparencyBuffer.h"

namespace trview
{
    namespace mocks
    {
        struct MockTransparencyBuffer : public ITransparencyBuffer
        {
            virtual ~MockTransparencyBuffer() = default;
            MOCK_METHOD(void, add, (const TransparentTriangle&), (override));
            MOCK_METHOD(void, sort, (const DirectX::SimpleMath::Vector3&), (override));
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, bool), (override));
            MOCK_METHOD(void, reset, (), (override));
        };
    }
}
