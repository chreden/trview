#pragma once

#include "../../Geometry/ITransparencyBuffer.h"

namespace trview
{
    namespace mocks
    {
        struct MockTransparencyBuffer : public ITransparencyBuffer
        {
            MockTransparencyBuffer();
            virtual ~MockTransparencyBuffer();
            MOCK_METHOD(void, add, (const Triangle&), (override));
            MOCK_METHOD(void, sort, (const DirectX::SimpleMath::Vector3&), (override));
            MOCK_METHOD(void, render, (const DirectX::SimpleMath::Matrix&, bool), (override));
            MOCK_METHOD(void, reset, (), (override));
        };
    }
}
