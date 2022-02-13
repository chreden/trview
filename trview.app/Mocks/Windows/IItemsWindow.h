#pragma once

#include "../../Windows/IItemsWindow.h"

namespace trview
{
    namespace mocks
    {
        class MockItemsWindow final : public IItemsWindow
        {
        public:
            virtual ~MockItemsWindow() = default;
            MOCK_METHOD(void, set_items, (const std::vector<Item>&));
            MOCK_METHOD(void, update_items, (const std::vector<Item>&));
            MOCK_METHOD(void, render, (bool));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&));
            MOCK_METHOD(void, clear_selected_item, ());
            MOCK_METHOD(void, set_current_room, (uint32_t));
            MOCK_METHOD(void, set_selected_item, (const Item&));
            MOCK_METHOD(std::optional<Item>, selected_item, (), (const));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
        };
    }
}