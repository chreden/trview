#pragma once

#include "../Application.h"

namespace trview
{
    namespace mocks
    {
        struct MockApplication : public IApplication
        {
            MockApplication();
            ~MockApplication();
            MOCK_METHOD(int, run, (), (override));
            MOCK_METHOD(std::weak_ptr<ILevel>, current_level, (), (const, override));
            MOCK_METHOD(std::shared_ptr<ILevel>, load, (const std::string&), (override));
            MOCK_METHOD(void, set_current_level, (const std::shared_ptr<ILevel>&, ILevel::OpenMode, bool), (override));
            MOCK_METHOD(UserSettings, settings, (), (const, override));
        };
    }
}
