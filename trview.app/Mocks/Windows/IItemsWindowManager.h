#pragma once

#include <trview.app/Windows/IItemsWindowManager.h>

namespace trview
{
    namespace mocks
    {
        class MockItemsWindowManager final : public IItemsWindowManager
        {
        public:
            MOCK_METHOD(void, render, (bool));
            MOCK_METHOD(void, set_items, (const std::vector<Item>&));
            MOCK_METHOD(void, set_item_visible, (const Item&, bool));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&));
            MOCK_METHOD(void, set_room, (uint32_t));
            MOCK_METHOD(void, set_selected_item, (const Item&));
            MOCK_METHOD(std::weak_ptr<IItemsWindow>, create_window, ());
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
