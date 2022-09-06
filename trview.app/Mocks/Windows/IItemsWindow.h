#pragma once

#include "../../Windows/IItemsWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockItemsWindow : public IItemsWindow
        {
            virtual ~MockItemsWindow() = default;
            MOCK_METHOD(void, set_items, (const std::vector<Item>&), (override));
            MOCK_METHOD(void, update_item, (const Item&), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&), (override));
            MOCK_METHOD(void, clear_selected_item, (), (override));
            MOCK_METHOD(void, set_current_room, (uint32_t), (override));
            MOCK_METHOD(void, set_selected_item, (const Item&), (override));
            MOCK_METHOD(std::optional<Item>, selected_item, (), (const, override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, set_level_version, (trlevel::LevelVersion), (override));
            MOCK_METHOD(void, set_model_checker, (const std::function<bool(uint32_t)>&), (override));
        };
    }
}