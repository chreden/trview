#pragma once

#include "../../Windows/ITriggersWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockTriggersWindowManager : public ITriggersWindowManager
        {
            MockTriggersWindowManager();
            virtual ~MockTriggersWindowManager();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_items, (const std::vector<std::weak_ptr<IItem>>&), (override));
            MOCK_METHOD(void, set_platform_and_version, (const trlevel::PlatformAndVersion&), (override));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&), (override));
            MOCK_METHOD(void, set_room, (const std::weak_ptr<IRoom>&), (override));
            MOCK_METHOD(void, set_selected_trigger, (const std::weak_ptr<ITrigger>&), (override));
            MOCK_METHOD(std::weak_ptr<ITriggersWindow>, create_window, (), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
