#pragma once

#include "../../Windows/Floordata/IFloordataWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockFloordataWindowManager : public IFloordataWindowManager
        {
            virtual ~MockFloordataWindowManager() = default;
            MOCK_METHOD(std::weak_ptr<IFloordataWindow>, create_window, (), (override));
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_room, (uint32_t), (override));
            MOCK_METHOD(void, set_floordata, (const std::vector<uint16_t>&), (override));
            MOCK_METHOD(void, set_items, (const std::vector<Item>&), (override));
            MOCK_METHOD(void, set_rooms, (const std::vector<std::weak_ptr<IRoom>>&), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
