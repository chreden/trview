#pragma once

#include <trview.app/Elements/IRoom.h>

namespace trview
{
    namespace mocks
    {
        struct MockRoom : public IRoom, public std::enable_shared_from_this<MockRoom>
        {
            MockRoom();
            virtual ~MockRoom();
            MOCK_METHOD(void, add_entity, (const std::weak_ptr<IItem>&), (override));
            MOCK_METHOD(void, add_light, (const std::weak_ptr<ILight>&), (override));
            MOCK_METHOD(void, add_trigger, (const std::weak_ptr<ITrigger>&), (override));
            MOCK_METHOD(void, add_camera_sink, (const std::weak_ptr<ICameraSink>&), (override));
            MOCK_METHOD(int16_t, alternate_group, (), (const, override));
            MOCK_METHOD(Colour, ambient, (), (const, override));
            MOCK_METHOD(int16_t, ambient_intensity_1, (), (const, override));
            MOCK_METHOD(int16_t, ambient_intensity_2, (), (const, override));
            MOCK_METHOD(AlternateMode, alternate_mode, (), (const, override));
            MOCK_METHOD(int16_t, alternate_room, (), (const, override));
            MOCK_METHOD(DirectX::BoundingBox, bounding_box, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, centre, (), (const, override));
            MOCK_METHOD(void, generate_sector_triangles, (), (override));
            MOCK_METHOD(void, generate_trigger_geometry, (), (override));
            MOCK_METHOD(void, get_contained_transparent_triangles, (ITransparencyBuffer&, const ICamera&, SelectionMode, RenderFilter), (override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, SelectionMode, RenderFilter), (override));
            MOCK_METHOD(RoomInfo, info, (), (const, override));
            MOCK_METHOD(int16_t, light_mode, (), (const, override));
            MOCK_METHOD(std::set<uint16_t>, neighbours, (), (const, override));
            MOCK_METHOD(uint16_t, num_x_sectors, (), (const, override));
            MOCK_METHOD(uint16_t, num_z_sectors, (), (const, override));
            MOCK_METHOD(uint32_t, number, (), (const, override));
            MOCK_METHOD(bool, outside, (), (const, override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&, PickFilter), (const, override));
            MOCK_METHOD(bool, quicksand, (), (const, override));
            MOCK_METHOD(void, render, (const ICamera&, SelectionMode, RenderFilter, const std::unordered_set<uint32_t>&), (override));
            MOCK_METHOD(void, render_bounding_boxes, (const ICamera&), (override));
            MOCK_METHOD(void, render_lights, (const ICamera&, const std::weak_ptr<ILight>&), (override));
            MOCK_METHOD(void, render_camera_sinks, (const ICamera&), (override));
            MOCK_METHOD(void, render_contained, (const ICamera&, SelectionMode, RenderFilter), (override));;
            MOCK_METHOD(const std::vector<std::shared_ptr<ISector>>, sectors, (), (const, override));
            MOCK_METHOD(void, set_is_alternate, (int16_t), (override));
            MOCK_METHOD(std::weak_ptr<ITrigger>, trigger_at, (int32_t, int32_t), (const, override));
            MOCK_METHOD(void, update_bounding_box, (), (override));
            MOCK_METHOD(bool, water, (), (const, override));
            MOCK_METHOD(bool, flag, (IRoom::Flag), (const, override));
            MOCK_METHOD(float, y_bottom, (), (const, override));
            MOCK_METHOD(float, y_top, (), (const, override));
            MOCK_METHOD(ISector::Portal, sector_portal, (int, int, int, int), (const, override));
            MOCK_METHOD(void, set_sector_triangle_rooms, (const std::vector<uint32_t>&), (override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));
            MOCK_METHOD(bool, visible, (), (const, override));
            MOCK_METHOD(void, set_visible, (bool visible), (override));
            MOCK_METHOD(std::vector<std::weak_ptr<ILight>>, lights, (), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<ICameraSink>>, camera_sinks, (), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<ITrigger>>, triggers, (), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<IItem>>, items, (), (const, override));
            MOCK_METHOD(void, set_level, (const std::weak_ptr<ILevel>&), (override));
            MOCK_METHOD(std::weak_ptr<ILevel>, level, (), (const, override));


            std::shared_ptr<MockRoom> with_number(uint32_t number)
            {
                ON_CALL(*this, number).WillByDefault(testing::Return(number));
                return shared_from_this();
            }

            std::shared_ptr<MockRoom> with_alternate_group(int16_t group)
            {
                ON_CALL(*this, alternate_group).WillByDefault(testing::Return(group));
                return shared_from_this();
            }

            std::shared_ptr<MockRoom> with_alternate_room(int16_t alternate)
            {
                ON_CALL(*this, alternate_room).WillByDefault(testing::Return(alternate));
                return shared_from_this();
            }
        };
    }
}
