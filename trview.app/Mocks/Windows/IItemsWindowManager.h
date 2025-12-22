#pragma once

#include "../../Windows/IItemsWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockItemsWindowManager : public IItemsWindowManager
        {
        public:
            MockItemsWindowManager();
            virtual ~MockItemsWindowManager();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_ng_plus, (bool), (override));
            MOCK_METHOD(void, set_room, (const std::weak_ptr<IRoom>&), (override));
            MOCK_METHOD(std::weak_ptr<IItemsWindow>, create_window, (), (override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(std::vector<std::weak_ptr<IItemsWindow>>, windows, (), (const, override));
        };
    }
}
