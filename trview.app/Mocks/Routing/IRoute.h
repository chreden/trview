#pragma once

#include <trview.app/Routing/IRoute.h>

namespace trview
{
    namespace mocks
    {
        class MockRoute : public IRoute
        {
        public:
            virtual ~MockRoute() = default;
            MOCK_METHOD(void, add, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, uint32_t), (override));
            MOCK_METHOD(void, add, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, uint32_t, IWaypoint::Type, uint32_t), (override));
            MOCK_METHOD(void, clear, (), (override));
            MOCK_METHOD(Colour, colour, (), (const, override));
            MOCK_METHOD(void, insert, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, uint32_t, uint32_t), (override));
            MOCK_METHOD(uint32_t, insert, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, uint32_t), (override));
            MOCK_METHOD(void, insert, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, uint32_t, uint32_t, IWaypoint::Type, uint32_t), (override));
            MOCK_METHOD(uint32_t, insert, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, uint32_t, IWaypoint::Type, uint32_t), (override));
            MOCK_METHOD(bool, is_unsaved, (), (const, override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
            MOCK_METHOD(void, remove, (uint32_t), (override));
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&), (override));
            MOCK_METHOD(uint32_t, selected_waypoint, (), (const, override));
            MOCK_METHOD(void, select_waypoint, (uint32_t), (override));
            MOCK_METHOD(void, set_colour, (const Colour&), (override));
            MOCK_METHOD(void, set_randomizer_enabled, (bool), (override));
            MOCK_METHOD(void, set_unsaved, (bool), (override));
            MOCK_METHOD(const IWaypoint&, waypoint, (uint32_t), (const, override));
            MOCK_METHOD(IWaypoint&, waypoint, (uint32_t), (override));
            MOCK_METHOD(uint32_t, waypoints, (), (const, override));
        };
    }
}
