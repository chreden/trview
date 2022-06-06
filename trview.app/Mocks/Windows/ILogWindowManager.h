#pragma once

#include "../../Windows/Log/ILogWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockLogWindowManager : public ILogWindowManager
        {
            virtual ~MockLogWindowManager() = default;
            MOCK_METHOD(std::weak_ptr<ILogWindow>, create_window, (), (override));
            MOCK_METHOD(void, render, (), (override));
        };
    }
}
