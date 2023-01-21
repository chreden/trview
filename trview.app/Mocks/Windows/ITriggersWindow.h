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
            MOCK_METHOD(void, set_current_room, (uint32_t), (override));
            MOCK_METHOD(void, set_items, (const std::vector<std::weak_ptr<IItem>>&), (override));
            MOCK_METHOD(void, set_selected_trigger, (const std::weak_ptr<ITrigger>&), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
