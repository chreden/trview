#pragma once

#include "../../UI/ILevelInfo.h"

namespace trview
{
    namespace mocks
    {
        struct MockLevelInfo : public ILevelInfo
        {
            MockLevelInfo();
            virtual ~MockLevelInfo();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_level, (const std::weak_ptr<ILevel>&), (override));
        };
    }
}
