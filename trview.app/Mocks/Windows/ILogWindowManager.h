#pragma once

#include "../../Windows/Log/ILogWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockLogWindowManager : public ILogWindowManager
        {
            MockLogWindowManager();
            virtual ~MockLogWindowManager();
            MOCK_METHOD(std::weak_ptr<ILogWindow>, create_window, (), (override));
            MOCK_METHOD(void, render, (), (override));
        };
    }
}
