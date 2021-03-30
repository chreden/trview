#pragma once

#include <trview.app/Geometry/ITransparencyBuffer.h>

namespace trview
{
    namespace mocks
    {
        class MockTransparencyBuffer final : public ITransparencyBuffer
        {
        public:
            MOCK_METHOD(void, add, (const TransparentTriangle&));
            MOCK_METHOD(void, sort, (const DirectX::SimpleMath::Vector3&));
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, bool));
            MOCK_METHOD(void, reset, ());
        };
    }
}
