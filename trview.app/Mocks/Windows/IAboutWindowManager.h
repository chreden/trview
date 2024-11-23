#pragma once

#include "../../Windows/About/IAboutWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockAboutWindowManager : public IAboutWindowManager
        {
            MockAboutWindowManager();
            virtual ~MockAboutWindowManager();
            MOCK_METHOD(void, create_window, (), (override));
            MOCK_METHOD(void, render, (), (override));
        };
    }
}
