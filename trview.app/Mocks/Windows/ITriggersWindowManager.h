#pragma once

#include <trview.app/Windows/ITriggersWindowManager.h>

namespace trview
{
    namespace mocks
    {
        class MockTriggersWindowManager final : public ITriggersWindowManager
        {
        public:
            MOCK_METHOD(void, render, (bool), (override));
            MOCK_METHOD(void, set_items, (const std::vector<Item>&), (override));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&), (override));
            MOCK_METHOD(void, set_trigger_visible, (const std::weak_ptr<ITrigger>& trigger, bool), (override));
            MOCK_METHOD(void, set_room, (uint32_t), (override));
            MOCK_METHOD(void, set_selected_trigger, (const std::weak_ptr<ITrigger>&), (override));
            MOCK_METHOD(std::weak_ptr<ITriggersWindow>, create_window, (), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
