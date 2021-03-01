#pragma once

#include "../../Menus/ILevelSwitcher.h"

namespace trview
{
    namespace mocks
    {
        class MockLevelSwitcher final : public ILevelSwitcher
        {
        public:
            virtual ~MockLevelSwitcher() = default;
            MOCK_METHOD(void, open_file, (const std::string&));
        };
    }
}
