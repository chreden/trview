#pragma once

#include <trview.app/Windows/IRoomsWindowManager.h>

namespace trview
{
    namespace mocks
    {
        class MockRoomsWindowManager final : public IRoomsWindowManager
        {
        public:
            MOCK_METHOD(void, render, (graphics::IDevice&, bool));
            MOCK_METHOD(void, set_items, (const std::vector<Item>&));
            MOCK_METHOD(void, set_room, (uint32_t));
            MOCK_METHOD(void, set_rooms, (const std::vector<Room*>&));
            MOCK_METHOD(void, set_selected_item, (const Item&));
            MOCK_METHOD(void, set_selected_trigger, (const Trigger* const ));
            MOCK_METHOD(void, set_triggers, (const std::vector<Trigger*>&));
            MOCK_METHOD(void, create_window, ());
        };
    }
}

