module;

#include <gmock/gmock.h>
#include <SimpleMath.h>

export module trview.app.mocks:MockTransparencyBuffer;

import trview.app;

namespace trview
{
    namespace mocks
    {
        export struct MockTransparencyBuffer : public ITransparencyBuffer
        {
            MockTransparencyBuffer(){}
            virtual ~MockTransparencyBuffer(){}
            MOCK_METHOD(void, add, (const Triangle&), (override));
            MOCK_METHOD(void, sort, (const DirectX::SimpleMath::Vector3&), (override));
            MOCK_METHOD(void, render, (const ICamera&, bool), (override));
            MOCK_METHOD(void, reset, (), (override));
        };
    }
}
