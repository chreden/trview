#pragma once

#include <trview.app/Elements/IRoom.h>

namespace trview
{
    namespace mocks
    {
        struct MockRoom final : public IRoom
        {
            virtual ~MockRoom() = default;
            MOCK_METHOD(void, add_entity, (const std::weak_ptr<IEntity>&), (override));
            MOCK_METHOD(void, add_trigger, (const std::weak_ptr<ITrigger>&), (override));
            MOCK_METHOD(int16_t, alternate_group, (), (const, override));
            MOCK_METHOD(AlternateMode, alternate_mode, (), (const, override));
            MOCK_METHOD(int16_t, alternate_room, (), (const, override));
            MOCK_METHOD(DirectX::BoundingBox, bounding_box, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, centre, (), (const, override));
            MOCK_METHOD(void, generate_trigger_geometry, (), (override));
            MOCK_METHOD(void, get_contained_transparent_triangles, (ITransparencyBuffer&, const ICamera&, SelectionMode, bool), (override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, SelectionMode, bool, bool), (override));
            MOCK_METHOD(RoomInfo, info, (), (const, override));
            MOCK_METHOD(std::set<uint16_t>, neighbours, (), (const, override));
            MOCK_METHOD(uint16_t, num_x_sectors, (), (const, override));
            MOCK_METHOD(uint16_t, num_z_sectors, (), (const, override));
            MOCK_METHOD(uint32_t, number, (), (const, override));
            MOCK_METHOD(bool, outside, (), (const, override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, PickFilter), (const, override));
            MOCK_METHOD(bool, quicksand, (), (const, override));
            MOCK_METHOD(void, render, (const ICamera&, SelectionMode, bool, bool), (override));
            MOCK_METHOD(void, render_bounding_boxes, (const ICamera&), (override));
            MOCK_METHOD(void, render_contained, (const ICamera&, SelectionMode, bool), (override));;
            MOCK_METHOD(const std::vector<std::shared_ptr<ISector>>, sectors, (), (const, override));
            MOCK_METHOD(void, set_is_alternate, (int16_t), (override));
            MOCK_METHOD(std::weak_ptr<ITrigger>, trigger_at, (int32_t, int32_t), (const, override));
            MOCK_METHOD(void, update_bounding_box, (), (override));
            MOCK_METHOD(bool, water, (), (const, override));
        };
    }
}
