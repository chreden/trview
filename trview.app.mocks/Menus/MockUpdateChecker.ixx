module;

#include <gmock/gmock.h>

export module trview.app.mocks:MockUpdateChecker;

import trview.app;

namespace trview
{
    namespace mocks
    {
        export struct MockUpdateChecker : public IUpdateChecker
        {
            MOCK_METHOD(void, check_for_updates, (), (override));
        };
    }
}
