#pragma once

#include "../../Windows/IRouteWindow.h"

namespace trview
{
    namespace mocks
    {
        struct MockRouteWindow : public IRouteWindow
        {
            MockRouteWindow();
            virtual ~MockRouteWindow();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_route, (const std::weak_ptr<IRoute>&), (override));
            MOCK_METHOD(void, select_waypoint, (const std::weak_ptr<IWaypoint>&), (override));
            MOCK_METHOD(void, set_items, (const std::vector<std::weak_ptr<IItem>>&), (override));
            MOCK_METHOD(void, set_rooms, (const std::vector<std::weak_ptr<IRoom>>&), (override));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&), (override));
            MOCK_METHOD(void, focus, (), (override));
            MOCK_METHOD(void, update, (float), (override));
        };
    }
}