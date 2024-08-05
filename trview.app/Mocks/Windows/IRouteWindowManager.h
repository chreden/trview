#pragma once

#include "../../Windows/IRouteWindowManager.h"

namespace trview
{
    namespace mocks
    {
        struct MockRouteWindowManager : public IRouteWindowManager
        {
            MockRouteWindowManager();
            virtual ~MockRouteWindowManager();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_route, (const std::weak_ptr<IRoute>&), (override));
            MOCK_METHOD(void, create_window, (), (override));
            MOCK_METHOD(void, set_items, (const std::vector<std::weak_ptr<IItem>>&), (override));
            MOCK_METHOD(void, set_rooms, (const std::vector<std::weak_ptr<IRoom>>&), (override));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&), (override));
            MOCK_METHOD(void, select_waypoint, (const std::weak_ptr<IWaypoint>&), (override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, set_randomizer_enabled, (bool), (override));
            MOCK_METHOD(void, set_randomizer_settings, (const RandomizerSettings&), (override));
            MOCK_METHOD(bool, is_window_open, (), (const, override));
        };
    }
}
