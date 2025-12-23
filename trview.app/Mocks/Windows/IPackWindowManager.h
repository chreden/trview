#pragma once

#include "../../Windows/Pack/IPackWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockPackWindowManager : public IPackWindowManager
        {
            MockPackWindowManager();
            ~MockPackWindowManager();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(std::weak_ptr<IPackWindow>, create_window, (), (override));
            MOCK_METHOD(void, set_level, (const std::weak_ptr<ILevel>&), (override));
        };
    }
}
