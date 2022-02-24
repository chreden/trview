#pragma once

#include "../../Menus/ILevelSwitcher.h"

namespace trview
{
    namespace mocks
    {
        struct MockLevelSwitcher : public ILevelSwitcher
        {
            virtual ~MockLevelSwitcher() = default;
            MOCK_METHOD(void, open_file, (const std::string&), (override));
        };
    }
}
