#pragma once

#include "../../Windows/IRoomsWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockRoomsWindowManager : public IRoomsWindowManager
        {
        public:
            MockRoomsWindowManager();
            virtual ~MockRoomsWindowManager();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_items, (const std::vector<std::weak_ptr<IItem>>&), (override));
            MOCK_METHOD(void, set_level_version, (trlevel::LevelVersion), (override));
            MOCK_METHOD(void, set_settings, (const UserSettings&), (override));
            MOCK_METHOD(void, set_room, (const std::weak_ptr<IRoom>&), (override));
            MOCK_METHOD(void, set_rooms, (const std::vector<std::weak_ptr<IRoom>>&), (override));
            MOCK_METHOD(void, set_selected_item, (const std::weak_ptr<IItem>&), (override));
            MOCK_METHOD(void, set_selected_trigger, (const std::weak_ptr<ITrigger>& ), (override));
            MOCK_METHOD(std::weak_ptr<IRoomsWindow>, create_window, (), (override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, set_floordata, (const std::vector<uint16_t>&), (override));
            MOCK_METHOD(void, set_selected_camera_sink, (const std::weak_ptr<ICameraSink>&), (override));
            MOCK_METHOD(void, set_selected_light, (const std::weak_ptr<ILight>&), (override));
            MOCK_METHOD(void, set_selected_sector, (const std::weak_ptr<ISector>&), (override));
            MOCK_METHOD(void, set_ng_plus, (bool), (override));
            MOCK_METHOD(void, set_trng, (bool), (override));
            MOCK_METHOD(std::vector<std::weak_ptr<IRoomsWindow>>, windows, (), (const, override));
        };
    }
}

