#pragma once

#include "../../Windows/IItemsWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockItemsWindowManager : public IItemsWindowManager
        {
        public:
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_items, (const std::vector<Item>&), (override));
            MOCK_METHOD(void, set_item_visible, (const Item&, bool), (override));
            MOCK_METHOD(void, set_level_version, (trlevel::LevelVersion), (override));
            MOCK_METHOD(void, set_model_checker, (const std::function<bool(uint32_t)>&), (override));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&), (override));
            MOCK_METHOD(void, set_room, (uint32_t), (override));
            MOCK_METHOD(void, set_selected_item, (const Item&), (override));
            MOCK_METHOD(std::weak_ptr<IItemsWindow>, create_window, (), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
