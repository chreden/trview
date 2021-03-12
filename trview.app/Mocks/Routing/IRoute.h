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
            MOCK_METHOD(void, add, (const DirectX::SimpleMath::Vector3&, uint32_t));
            MOCK_METHOD(void, add, (const DirectX::SimpleMath::Vector3&, uint32_t, Waypoint::Type, uint32_t));
            MOCK_METHOD(void, clear, ());
            MOCK_METHOD(Colour, colour, (), (const));
            MOCK_METHOD(void, insert, (const DirectX::SimpleMath::Vector3&, uint32_t, uint32_t));
            MOCK_METHOD(uint32_t, insert, (const DirectX::SimpleMath::Vector3&, uint32_t));
            MOCK_METHOD(void, insert, (const DirectX::SimpleMath::Vector3&, uint32_t, uint32_t, Waypoint::Type, uint32_t));
            MOCK_METHOD(uint32_t, insert, (const DirectX::SimpleMath::Vector3&, uint32_t, Waypoint::Type, uint32_t));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const));
            MOCK_METHOD(void, remove, (uint32_t));
            MOCK_METHOD(void, render, (const graphics::Device&, const ICamera&, const ILevelTextureStorage&));
            MOCK_METHOD(uint32_t, selected_waypoint, (), (const));
            MOCK_METHOD(void, select_waypoint, (uint32_t));
            MOCK_METHOD(void, set_colour, (const Colour&));
            MOCK_METHOD(const Waypoint&, waypoint, (uint32_t), (const));
            MOCK_METHOD(Waypoint&, waypoint, (uint32_t));
            MOCK_METHOD(uint32_t, waypoints, (), (const));
        };
    }
}
