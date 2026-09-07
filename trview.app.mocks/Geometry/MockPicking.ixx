module;

#include <gmock/gmock.h>

export module trview.app.mocks:MockPicking;

import trview.app;

namespace trview
{
    namespace mocks
    {
        export struct MockPicking : public IPicking
        {
            MOCK_METHOD(void, pick, (const ICamera&), (override));
        };
    }
}
