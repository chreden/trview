#pragma once

#include "../../Windows/IWindows.h"

namespace trview
{
    namespace mocks
    {
        struct MockWindows : public IWindows
        {
            MockWindows();
            virtual ~MockWindows();
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, select, (const std::weak_ptr<IWaypoint>&), (override));
            MOCK_METHOD(void, setup, (const UserSettings&), (override));
        };
    }
}
