#pragma once

#include "../../Windows/Floordata/IFloordataWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockFloordataWindow : public IFloordataWindow
        {
            virtual ~MockFloordataWindow() = default;
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_current_room, (uint32_t), (override));
            MOCK_METHOD(void, set_floordata, (const std::vector<uint16_t>&), (override));
            MOCK_METHOD(void, set_items, (const std::vector<Item>&), (override));
            MOCK_METHOD(void, set_number, (int32_t), (override));
            MOCK_METHOD(void, set_rooms, (const std::vector<std::weak_ptr<IRoom>>&), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}
