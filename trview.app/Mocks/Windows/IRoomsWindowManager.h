#pragma once

#include <trview.app/Windows/IRoomsWindowManager.h>

namespace trview
{
    namespace mocks
    {
        class MockRoomsWindowManager final : public IRoomsWindowManager
        {
        public:
            MOCK_METHOD(void, render, (bool));
            MOCK_METHOD(void, set_items, (const std::vector<Item>&));
            MOCK_METHOD(void, set_room, (uint32_t));
            MOCK_METHOD(void, set_rooms, (const std::vector<std::weak_ptr<IRoom>>&));
            MOCK_METHOD(void, set_selected_item, (const Item&));
            MOCK_METHOD(void, set_selected_trigger, (const std::weak_ptr<ITrigger>& ));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&));
            MOCK_METHOD(std::weak_ptr<IRoomsWindow>, create_window, ());
        };
    }
}

