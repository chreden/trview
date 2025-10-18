#pragma once

#include "../../Windows/ITriggersWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockTriggersWindow : public ITriggersWindow
        {
            MockTriggersWindow();
            virtual ~MockTriggersWindow();
            MOCK_METHOD(void, clear_selected_trigger, (), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(std::weak_ptr<ITrigger>, selected_trigger, (), (const, override));
            MOCK_METHOD(void, set_current_room, (const std::weak_ptr<IRoom>&), (override));
            MOCK_METHOD(void, set_items, (const std::vector<std::weak_ptr<IItem>>&), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, set_platform_and_version, (const trlevel::PlatformAndVersion&), (override));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
