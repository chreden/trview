#pragma once

#include <trview.app/Elements/ISector.h>

namespace trview
{
    namespace mocks
    {
        struct MockSector : public ISector
        {
            MockSector();
            virtual ~MockSector();
            MOCK_METHOD(std::uint16_t, portal, (), (const, override));
            MOCK_METHOD(int, id, (), (const, override));
            MOCK_METHOD(std::set<std::uint16_t>, neighbours, (), (const, override));
            MOCK_METHOD(std::uint16_t, room_below, (), (const, override));
            MOCK_METHOD(std::uint16_t, room_above, (), (const, override));
            MOCK_METHOD(SectorFlag, flags, (), (const, override));
            MOCK_METHOD(uint32_t, floordata_index, (), (const, override));
            MOCK_METHOD(TriggerInfo, trigger, (), (const, override));
            MOCK_METHOD(uint16_t, x, (), (const, override));
            MOCK_METHOD(uint16_t, z, (), (const, override));
            MOCK_METHOD((std::array<float, 4>), corners, (), (const, override));
            MOCK_METHOD((std::array<float, 4>), ceiling_corners, (), (const, override));
            MOCK_METHOD((DirectX::SimpleMath::Vector3), corner, (Corner), (const, override));
            MOCK_METHOD((DirectX::SimpleMath::Vector3), ceiling, (Corner), (const, override));
            MOCK_METHOD(uint32_t, room, (), (const, override));
            MOCK_METHOD(TriangulationDirection, triangulation_function, (), (const, override));
            MOCK_METHOD(std::vector<Triangle>, triangles, (), (const, override));
            MOCK_METHOD(bool, is_floor, (), (const, override));
            MOCK_METHOD(bool, is_wall, (), (const, override));
            MOCK_METHOD(bool, is_portal, (), (const, override));
            MOCK_METHOD(bool, is_ceiling, (), (const, override));

            MOCK_METHOD(void, generate_triangles, (), (override));
            MOCK_METHOD(void, add_triangle, (const ISector::Portal&, const Triangle&, std::unordered_set<uint32_t>), (override));
            MOCK_METHOD(void, add_flag, (SectorFlag), (override));
        };
    }
}
