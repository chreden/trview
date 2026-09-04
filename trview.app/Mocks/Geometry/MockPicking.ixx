module;

#include <gmock/gmock.h>

export module trview.app:MockPicking;

import :IPicking;

namespace trview
{
    namespace mocks
    {
        struct MockPicking : public IPicking
        {
            MockPicking();
            virtual ~MockPicking();
            MOCK_METHOD(void, pick, (const ICamera&), (override));
        };
    }
}
