#pragma once

#include "../../Windows/Plugins/IPluginsWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockPluginsWindowManager : public IPluginsWindowManager
        {
            MockPluginsWindowManager();
            ~MockPluginsWindowManager();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(std::weak_ptr<IPluginsWindow>, create_window, (), (override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, set_settings, (const UserSettings&), (override));
        };
    }
}
