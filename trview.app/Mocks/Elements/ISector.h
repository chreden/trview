#pragma once

#include <trview.app/Elements/ISector.h>

namespace trview
{
    namespace mocks
    {
        struct MockSector final : public ISector
        {
            virtual ~MockSector() = default;
            MOCK_METHOD(std::uint16_t, portal, (), (const, override));
            MOCK_METHOD(int, id, (), (const, override));
            MOCK_METHOD(std::set<std::uint16_t>, neighbours, (), (const, override));
            MOCK_METHOD(std::uint16_t, room_below, (), (const, override));
            MOCK_METHOD(std::uint16_t, room_above, (), (const, override));
            MOCK_METHOD(uint16_t, flags, (), (const, override));
            MOCK_METHOD(TriggerInfo, trigger, (), (const, override));
            MOCK_METHOD(uint16_t, x, (), (const, override));
            MOCK_METHOD(uint16_t, z, (), (const, override));
            MOCK_METHOD((std::array<float, 4>), corners, (), (const, override));
            MOCK_METHOD(uint32_t, room, (), (const, override));
            MOCK_METHOD(TriangulationDirection, triangulation_function, (), (const, override));
            MOCK_METHOD(std::vector<DirectX::SimpleMath::Vector3>, triangles, (), (const, override));
            MOCK_METHOD(bool, is_floor, (), (const, override));
        };
    }
}
