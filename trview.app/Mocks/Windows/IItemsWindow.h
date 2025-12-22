#pragma once

#include "../../Windows/IItemsWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockItemsWindow : public IItemsWindow
        {
            MockItemsWindow();
            virtual ~MockItemsWindow();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, clear_selected_item, (), (override));
            MOCK_METHOD(void, set_current_room, (const std::weak_ptr<IRoom>&), (override));
            MOCK_METHOD(std::weak_ptr<IItem>, selected_item, (), (const, override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, set_filters, (std::vector<Filters<IItem>::Filter>), (override));
            MOCK_METHOD(std::string, name, (), (const, override));
        };
    }
}