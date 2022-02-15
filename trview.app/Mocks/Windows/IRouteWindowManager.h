#pragma once

#include <trview.app/Windows/IRouteWindowManager.h>

namespace trview
{
    namespace mocks
    {
        class MockRouteWindowManager final : public IRouteWindowManager
        {
        public:
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_route, (IRoute*), (override));
            MOCK_METHOD(void, create_window, (), (override));
            MOCK_METHOD(void, set_items, (const std::vector<Item>&), (override));
            MOCK_METHOD(void, set_rooms, (const std::vector<std::weak_ptr<IRoom>>&), (override));
            MOCK_METHOD(void, set_triggers, (const std::vector<std::weak_ptr<ITrigger>>&), (override));
            MOCK_METHOD(void, select_waypoint, (uint32_t), (override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, set_randomizer_enabled, (bool), (override));
            MOCK_METHOD(void, set_randomizer_settings, (const RandomizerSettings&), (override));
        };
    }
}
