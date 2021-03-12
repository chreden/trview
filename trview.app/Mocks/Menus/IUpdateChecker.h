#pragma once

#include "../../Menus/IUpdateChecker.h"

namespace trview
{
    namespace mocks
    {
        class MockUpdateChecker final : public IUpdateChecker
        {
        public:
            virtual ~MockUpdateChecker() = default;
            MOCK_METHOD(void, check_for_updates, ());
        };
    }
}
