#pragma once

#include "../../Elements/ILevel.h"

namespace trview
{
    namespace mocks
    {
        struct MockLevel : public ILevel
        {
            MockLevel();
            virtual ~MockLevel();
            MOCK_METHOD(bool, alternate_group, (uint32_t), (const, override));
            MOCK_METHOD(std::set<uint32_t>, alternate_groups, (), (const, override));
            MOCK_METHOD(bool, alternate_mode, (), (const, override));
            MOCK_METHOD(bool, any_alternates, (), (const, override));
            MOCK_METHOD(std::string, filename, (), (const, override));
            MOCK_METHOD(bool, has_model, (uint32_t), (const, override));
            MOCK_METHOD(bool, highlight_mode_enabled, (RoomHighlightMode), (const, override));
            MOCK_METHOD(std::optional<Item>, item, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<Item>, items, (), (const, override));
            MOCK_METHOD(std::vector<graphics::Texture>, level_textures, (), (const, override));
            MOCK_METHOD(std::weak_ptr<ILight>, light, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<ILight>>, lights, (), (const, override));
            MOCK_METHOD(uint32_t, neighbour_depth, (), (const, override));
            MOCK_METHOD(uint32_t, number_of_rooms, (), (const, override));
            MOCK_METHOD(void, on_camera_moved, (), (override));
            MOCK_METHOD(PickResult, pick, (const ICamera&, const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
            MOCK_METHOD(void, render, (const ICamera&, bool), (override));
            MOCK_METHOD(void, render_transparency, (const ICamera&), (override));
            MOCK_METHOD(std::weak_ptr<IRoom>, room, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<RoomInfo>, room_info, (), (const, override));
            MOCK_METHOD(RoomInfo, room_info, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<IRoom>>, rooms, (), (const, override));
            MOCK_METHOD(std::optional<uint32_t>, selected_item, (), (const, override));
            MOCK_METHOD(std::optional<uint32_t>, selected_light, (), (const, override));
            MOCK_METHOD(uint16_t, selected_room, (), (const, override));
            MOCK_METHOD(std::optional<uint32_t>, selected_trigger, (), (const, override));
            MOCK_METHOD(void, set_alternate_mode, (bool), (override));
            MOCK_METHOD(void, set_alternate_group, (uint32_t, bool), (override));
            MOCK_METHOD(void, set_filename, (const std::string&), (override));
            MOCK_METHOD(void, set_highlight_mode, (RoomHighlightMode, bool), (override));
            MOCK_METHOD(void, set_item_visibility, (uint32_t, bool), (override));
            MOCK_METHOD(void, set_selected_trigger, (uint32_t), (override));
            MOCK_METHOD(void, set_selected_light, (uint32_t), (override));
            MOCK_METHOD(void, set_show_geometry, (bool), (override));
            MOCK_METHOD(void, set_show_triggers, (bool), (override));
            MOCK_METHOD(void, set_show_water, (bool), (override));
            MOCK_METHOD(void, set_show_wireframe, (bool), (override));
            MOCK_METHOD(void, set_show_bounding_boxes, (bool), (override));
            MOCK_METHOD(void, set_show_lights, (bool), (override));
            MOCK_METHOD(void, set_show_items, (bool), (override));
            MOCK_METHOD(void, set_show_rooms, (bool), (override));
            MOCK_METHOD(void, set_trigger_visibility, (uint32_t, bool), (override));
            MOCK_METHOD(void, set_neighbour_depth, (uint32_t), (override));
            MOCK_METHOD(void, set_selected_room, (uint16_t), (override));
            MOCK_METHOD(void, set_selected_item, (uint32_t), (override));
            MOCK_METHOD(void, set_light_visibility, (uint32_t, bool), (override));
            MOCK_METHOD(void, set_room_visibility, (uint32_t, bool), (override));
            MOCK_METHOD(bool, show_geometry, (), (const, override));
            MOCK_METHOD(bool, show_lights, (), (const, override));
            MOCK_METHOD(bool, show_triggers, (), (const, override));
            MOCK_METHOD(bool, show_items, (), (const, override));
            MOCK_METHOD(std::shared_ptr<ILevelTextureStorage>, texture_storage, (), (const, override));
            MOCK_METHOD(std::weak_ptr<ITrigger>, trigger, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<ITrigger>>, triggers, (), (const, override));
            MOCK_METHOD(trlevel::LevelVersion, version, (), (const, override));
            MOCK_METHOD(MapColours, map_colours, (), (const, override));
            MOCK_METHOD(void, set_map_colours, (const MapColours&), (override));
        };
    }
}
