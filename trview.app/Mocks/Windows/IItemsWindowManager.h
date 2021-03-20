#pragma once

#include <trview.app/Windows/IItemsWindowManager.h>

namespace trview
{
    namespace mocks
    {
        class MockItemsWindowManager : public IItemsWindowManager
        {
        public:
            MOCK_METHOD(void, render, (graphics::Device&, bool));
            MOCK_METHOD(void, set_items, (const std::vector<Item>&));
            MOCK_METHOD(void, set_item_visible, (const Item&, bool));
            MOCK_METHOD(void, set_triggers, (const std::vector<Trigger*>&));
            MOCK_METHOD(void, set_room, (uint32_t));
            MOCK_METHOD(void, set_selected_item, (const Item&));
            MOCK_METHOD(ItemsWindow*, create_window, ());
        };
    }
}
