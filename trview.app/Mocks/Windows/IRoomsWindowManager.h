#pragma once

#include "../../Windows/IRoomsWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockRoomsWindowManager : public IRoomsWindowManager
        {
        public:
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_items, (const std::vector<Item>&), (override));
            MOCK_METHOD(void, set_level_version, (trlevel::LevelVersion), (override));
            MOCK_METHOD(void, set_room, (uint32_t), (override));
            MOCK_METHOD(void, set_rooms, (const std::vector<std::weak_ptr<IRoom>>&), (override));
            MOCK_METHOD(void, set_selected_item, (const Item&), (override));
            MOCK_METHOD(void, set_selected_trigger, (const std::weak_ptr<ITrigger>& ), (override));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&), (override));
            MOCK_METHOD(std::weak_ptr<IRoomsWindow>, create_window, (), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}

