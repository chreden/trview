#pragma once

#include <trview.app/Windows/IRoomsWindow.h>

namespace trview
{
    namespace mocks
    {
        struct MockRoomsWindow : public IRoomsWindow
        {
            virtual ~MockRoomsWindow() = default;
            MOCK_METHOD(void, clear_selected_trigger, (), (override));
            MOCK_METHOD(void, render, (bool), (override));
            MOCK_METHOD(void, set_current_room, (uint32_t), (override));
            MOCK_METHOD(void, set_items, (const std::vector<Item>&), (override));
            MOCK_METHOD(void, set_rooms, (const std::vector<std::weak_ptr<IRoom>>&), (override));
            MOCK_METHOD(void, set_selected_item, (const Item&), (override));
            MOCK_METHOD(void, set_selected_trigger, (const std::weak_ptr<ITrigger>&), (override));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}