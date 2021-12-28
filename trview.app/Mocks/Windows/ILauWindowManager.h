#pragma once

#include "../../Windows/ILauWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockLauWindowManager : public ILauWindowManager
        {
            virtual ~MockLauWindowManager() = default;
            MOCK_METHOD(void, render, (bool), (override));
            MOCK_METHOD(std::weak_ptr<ILauWindow>, create_window, (), (override));
        };
    }
}
