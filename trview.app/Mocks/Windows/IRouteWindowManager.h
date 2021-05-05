#pragma once

#include <trview.app/Windows/IRouteWindowManager.h>

namespace trview
{
    namespace mocks
    {
        class MockRouteWindowManager final : public IRouteWindowManager
        {
        public:
            MOCK_METHOD(void, render, (bool));
            MOCK_METHOD(void, set_route, (IRoute*));
            MOCK_METHOD(void, create_window, ());
            MOCK_METHOD(void, set_items, (const std::vector<Item>&));
            MOCK_METHOD(void, set_rooms, (const std::vector<std::weak_ptr<IRoom>>&) );
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&));
            MOCK_METHOD(void, select_waypoint, (uint32_t));
        };
    }
}
