#pragma once

#include "../../Menus/IUpdateChecker.h"

namespace trview
{
    namespace mocks
    {
        struct MockUpdateChecker : public IUpdateChecker
        {
            virtual ~MockUpdateChecker() = default;
            MOCK_METHOD(void, check_for_updates, (), (override));
        };
    }
}
