#pragma once

#include <trview.app/Elements/ISector.h>

namespace trview
{
    namespace mocks
    {
        struct MockSector : public ISector, public std::enable_shared_from_this<MockSector>
        {
            MockSector();
            virtual ~MockSector();
            MOCK_METHOD(std::vector<std::uint16_t>, portals, (), (const, override));
            MOCK_METHOD(int, id, (), (const, override));
            MOCK_METHOD(std::set<std::uint16_t>, neighbours, (), (const, override));
            MOCK_METHOD(std::uint16_t, room_below, (), (const, override));
            MOCK_METHOD(std::uint16_t, room_above, (), (const, override));
            MOCK_METHOD(SectorFlag, flags, (), (const, override));
            MOCK_METHOD(uint32_t, floordata_index, (), (const, override));
            MOCK_METHOD(TriggerInfo, trigger_info, (), (const, override));
            MOCK_METHOD(uint16_t, x, (), (const, override));
            MOCK_METHOD(uint16_t, z, (), (const, override));
            MOCK_METHOD((std::array<float, 4>), corners, (), (const, override));
            MOCK_METHOD((std::array<float, 4>), ceiling_corners, (), (const, override));
            MOCK_METHOD((DirectX::SimpleMath::Vector3), corner, (Corner), (const, override));
            MOCK_METHOD((DirectX::SimpleMath::Vector3), ceiling, (Corner), (const, override));
            MOCK_METHOD(std::weak_ptr<IRoom>, room, (), (const, override));
            MOCK_METHOD(TriangulationDirection, triangulation, (), (const, override));
            MOCK_METHOD(std::vector<Triangle>, triangles, (), (const, override));
            MOCK_METHOD(bool, is_floor, (), (const, override));
            MOCK_METHOD(bool, is_wall, (), (const, override));
            MOCK_METHOD(bool, is_portal, (), (const, override));
            MOCK_METHOD(bool, is_ceiling, (), (const, override));

            MOCK_METHOD(void, generate_triangles, (), (override));
            MOCK_METHOD(void, add_triangle, (const ISector::Portal&, const Triangle&, std::unordered_set<uint32_t>), (override));
            MOCK_METHOD(void, add_flag, (SectorFlag), (override));
            MOCK_METHOD(void, set_trigger, (const std::weak_ptr<ITrigger>&), (override));
            MOCK_METHOD(int8_t, tilt_x, (), (const, override));
            MOCK_METHOD(int8_t, tilt_z, (), (const, override));
            MOCK_METHOD(std::weak_ptr<ITrigger>, trigger, (), (const, override));
            MOCK_METHOD(TriangulationDirection, ceiling_triangulation, (), (const, override));
            MOCK_METHOD(uint32_t, number, (), (const, override));
            MOCK_METHOD(uint16_t, material, (), (const, override));
            MOCK_METHOD(uint16_t, box_index, (), (const, override));
            MOCK_METHOD(bool, stopper, (), (const, override));
            MOCK_METHOD(int8_t, floor, (), (const, override));
            MOCK_METHOD(int8_t, ceiling, (), (const, override));
            MOCK_METHOD(int32_t, filterable_index, (), (const, override));

            std::shared_ptr<MockSector> with_ceiling_triangulation(TriangulationDirection triangulation)
            {
                ON_CALL(*this, ceiling_triangulation).WillByDefault(testing::Return(triangulation));
                return shared_from_this();
            }

            std::shared_ptr<MockSector> with_flags(SectorFlag flags)
            {
                ON_CALL(*this, flags).WillByDefault(testing::Return(flags));
                return shared_from_this();
            }

            std::shared_ptr<MockSector> with_room_above(uint16_t above)
            {
                ON_CALL(*this, room_above).WillByDefault(testing::Return(above));
                return shared_from_this();
            }

            std::shared_ptr<MockSector> with_room_below(uint16_t below)
            {
                ON_CALL(*this, room_below).WillByDefault(testing::Return(below));
                return shared_from_this();
            }

            std::shared_ptr<MockSector> with_id(uint32_t number)
            {
                ON_CALL(*this, id).WillByDefault(testing::Return(number));
                return shared_from_this();
            }

            std::shared_ptr<MockSector> with_portal(uint16_t portal)
            {
                ON_CALL(*this, portals).WillByDefault(testing::Return(std::vector<uint16_t>{ portal }));
                return shared_from_this();
            }

            std::shared_ptr<MockSector> with_portals(const std::vector<uint16_t>& portals)
            {
                ON_CALL(*this, portals).WillByDefault(testing::Return(portals));
                return shared_from_this();
            }

            std::shared_ptr<MockSector> with_room(const std::weak_ptr<IRoom>& room)
            {
                ON_CALL(*this, room).WillByDefault(testing::Return(room));
                return shared_from_this();
            }

            std::shared_ptr<MockSector> with_triangulation(TriangulationDirection triangulation)
            {
                ON_CALL(*this, triangulation).WillByDefault(testing::Return(triangulation));
                return shared_from_this();
            }

            std::shared_ptr<MockSector> with_trigger(const std::weak_ptr<ITrigger>& trigger)
            {
                ON_CALL(*this, trigger).WillByDefault(testing::Return(trigger));
                return shared_from_this();
            }

            std::shared_ptr<MockSector> with_x(uint16_t x)
            {
                ON_CALL(*this, x).WillByDefault(testing::Return(x));
                return shared_from_this();
            }

            std::shared_ptr<MockSector> with_z(uint16_t z)
            {
                ON_CALL(*this, z).WillByDefault(testing::Return(z));
                return shared_from_this();
            }
        };
    }
}
