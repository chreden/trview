#pragma once

#include "../../Menus/IUpdateChecker.h"

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
