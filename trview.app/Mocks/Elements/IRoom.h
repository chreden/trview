#pragma once

#include <trview.app/Elements/IRoom.h>

namespace trview
{
    namespace mocks
    {
        struct MockRoom final : public IRoom
        {
            virtual ~MockRoom() = default;
            MOCK_METHOD(void, add_entity, (IEntity*));
            MOCK_METHOD(void, add_trigger, (Trigger*));
            MOCK_METHOD(int16_t, alternate_group, (), (const));
            MOCK_METHOD(AlternateMode, alternate_mode, (), (const));
            MOCK_METHOD(int16_t, alternate_room, (), (const));
            MOCK_METHOD(DirectX::BoundingBox, bounding_box, (), (const));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, centre, (), (const));
            MOCK_METHOD(void, generate_trigger_geometry, ());
            MOCK_METHOD(void, get_contained_transparent_triangles, (ITransparencyBuffer&, const ICamera&, SelectionMode, bool, bool));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, SelectionMode, bool, bool));
            MOCK_METHOD(RoomInfo, info, (), (const));
            MOCK_METHOD(std::set<uint16_t>, neighbours, (), (const));
            MOCK_METHOD(uint16_t, num_x_sectors, (), (const));
            MOCK_METHOD(uint16_t, num_z_sectors, (), (const));
            MOCK_METHOD(uint32_t, number, (), (const));
            MOCK_METHOD(bool, outside, (), (const));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, bool, bool, bool, bool), (const));
            MOCK_METHOD(bool, quicksand, (), (const));
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, SelectionMode, bool, bool));
            MOCK_METHOD(void, render_contained, (const ICamera&, const ILevelTextureStorage&, SelectionMode, bool, bool));;
            MOCK_METHOD(const std::vector<std::shared_ptr<Sector>>, sectors, (), (const));
            MOCK_METHOD(void, set_is_alternate, (int16_t));
            MOCK_METHOD(Trigger*, trigger_at, (int32_t, int32_t), (const));
            MOCK_METHOD(void, update_bounding_box, ());
            MOCK_METHOD(bool, water, (), (const));
        };
    }
}
