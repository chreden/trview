#pragma once

#include <trview.app/Windows/ILightsWindowManager.h>

namespace trview
{
    namespace mocks
    {
        struct MockLightsWindowManager : public ILightsWindowManager
        {
            MockLightsWindowManager();
            virtual ~MockLightsWindowManager();
            MOCK_METHOD(std::weak_ptr<ILightsWindow>, create_window, (), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
