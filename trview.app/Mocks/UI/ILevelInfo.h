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
        };
    }
}
