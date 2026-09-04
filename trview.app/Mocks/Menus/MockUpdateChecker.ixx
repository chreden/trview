module;

#include <gmock/gmock.h>

export module trview.app:MockUpdateChecker;

import :IUpdateChecker;

namespace trview
{
    namespace mocks
    {
        struct MockUpdateChecker : public IUpdateChecker
        {
            MockUpdateChecker();
            virtual ~MockUpdateChecker();
            MOCK_METHOD(void, check_for_updates, (), (override));
        };
    }
}
