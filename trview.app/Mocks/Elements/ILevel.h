#pragma once

#include "../../Elements/ILevel.h"

namespace trview
{
    namespace mocks
    {
        class MockLevel final : public ILevel
        {
        public:
            MOCK_METHOD(bool, alternate_group, (uint32_t), (const));
            MOCK_METHOD(std::set<uint32_t>, alternate_groups, (), (const));
            MOCK_METHOD(bool, alternate_mode, (), (const));
            MOCK_METHOD(bool, any_alternates, (), (const));
            MOCK_METHOD(std::string, filename, (), (const));
            MOCK_METHOD(bool, highlight_mode_enabled, (RoomHighlightMode), (const));
            MOCK_METHOD(std::vector<Item>, items, (), (const));
            MOCK_METHOD(std::vector<graphics::Texture>, level_textures, (), (const));
            MOCK_METHOD(uint32_t, number_of_rooms, (), (const));
            MOCK_METHOD(void, on_camera_moved, ());
            MOCK_METHOD(PickResult, pick, (const ICamera&, const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const));
            MOCK_METHOD(void, render, (const graphics::IDevice&, const ICamera&, bool));
            MOCK_METHOD(void, render_transparency, (const graphics::IDevice&, const ICamera&));
            MOCK_METHOD(Room*, room, (std::size_t), (const));
            MOCK_METHOD(std::vector<RoomInfo>, room_info, (), (const));
            MOCK_METHOD(RoomInfo, room_info, (uint32_t), (const));
            MOCK_METHOD(std::vector<Room*>, rooms, (), (const));
            MOCK_METHOD(uint16_t, selected_room, (), (const));
            MOCK_METHOD(void, set_alternate_mode, (bool));
            MOCK_METHOD(void, set_alternate_group, (uint32_t, bool));
            MOCK_METHOD(void, set_filename, (const std::string&));
            MOCK_METHOD(void, set_highlight_mode, (RoomHighlightMode, bool));
            MOCK_METHOD(void, set_item_visibility, (uint32_t, bool));
            MOCK_METHOD(void, set_selected_trigger, (uint32_t));
            MOCK_METHOD(void, set_show_hidden_geometry, (bool));
            MOCK_METHOD(void, set_show_triggers, (bool));
            MOCK_METHOD(void, set_show_water, (bool));
            MOCK_METHOD(void, set_show_wireframe, (bool));
            MOCK_METHOD(void, set_trigger_visibility, (uint32_t, bool));
            MOCK_METHOD(void, set_neighbour_depth, (uint32_t));
            MOCK_METHOD(void, set_selected_room, (uint16_t));
            MOCK_METHOD(void, set_selected_item, (uint32_t));
            MOCK_METHOD(bool, show_hidden_geometry, (), (const));
            MOCK_METHOD(bool, show_triggers, (), (const));
            MOCK_METHOD(const ILevelTextureStorage&, texture_storage, (), (const));
            MOCK_METHOD(std::vector<Trigger*>, triggers, (), (const));
            MOCK_METHOD(trlevel::LevelVersion, version, (), (const));
        };
    }
}
