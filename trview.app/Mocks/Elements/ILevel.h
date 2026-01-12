#pragma once

#include "../../Elements/ILevel.h"

namespace trview
{
    namespace mocks
    {
        struct MockLevel : public ILevel, public std::enable_shared_from_this<MockLevel>
        {
            MockLevel();
            virtual ~MockLevel();
            MOCK_METHOD(void, add_scriptable, (const std::weak_ptr<IScriptable>&), (override));
            MOCK_METHOD(bool, alternate_group, (uint32_t), (const, override));
            MOCK_METHOD(std::set<uint32_t>, alternate_groups, (), (const, override));
            MOCK_METHOD(bool, alternate_mode, (), (const, override));
            MOCK_METHOD(bool, any_alternates, (), (const, override));
            MOCK_METHOD(std::weak_ptr<ICameraSink>, camera_sink, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<ICameraSink>>, camera_sinks, (), (const, override));
            MOCK_METHOD(std::string, filename, (), (const, override));
            MOCK_METHOD(bool, has_model, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<uint16_t>, floor_data, (), (const, override));
            MOCK_METHOD(bool, highlight_mode_enabled, (RoomHighlightMode), (const, override));
            MOCK_METHOD(bool, is_in_visible_set, (const std::weak_ptr<IRoom>&), (const, override));
            MOCK_METHOD(std::weak_ptr<IItem>, item, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<IItem>>, items, (), (const, override));
            MOCK_METHOD(std::vector<graphics::Texture>, level_textures, (), (const, override));
            MOCK_METHOD(std::weak_ptr<ILight>, light, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<ILight>>, lights, (), (const, override));
            MOCK_METHOD(std::string, name, (), (const, override));
            MOCK_METHOD(uint32_t, neighbour_depth, (), (const, override));
            MOCK_METHOD(uint32_t, number_of_rooms, (), (const, override));
            MOCK_METHOD(void, on_camera_moved, (), (override));
            MOCK_METHOD(PickResult, pick, (const ICamera&, const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
            MOCK_METHOD(void, render, (const ICamera&, bool), (override));
            MOCK_METHOD(void, render_transparency, (const ICamera&), (override));
            MOCK_METHOD(std::weak_ptr<IRoom>, room, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<IRoom>>, rooms, (), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<IScriptable>>, scriptables, (), (const, override));
            MOCK_METHOD(std::optional<uint32_t>, selected_item, (), (const, override));
            MOCK_METHOD(std::optional<uint32_t>, selected_light, (), (const, override));
            MOCK_METHOD(std::weak_ptr<IRoom>, selected_room, (), (const, override));
            MOCK_METHOD(std::optional<uint32_t>, selected_trigger, (), (const, override));
            MOCK_METHOD(void, set_alternate_mode, (bool), (override));
            MOCK_METHOD(void, set_alternate_group, (uint32_t, bool), (override));
            MOCK_METHOD(void, set_filename, (const std::string&), (override));
            MOCK_METHOD(void, set_highlight_mode, (RoomHighlightMode, bool), (override));
            MOCK_METHOD(void, set_selected_trigger, (uint32_t), (override));
            MOCK_METHOD(void, set_selected_light, (uint32_t), (override));
            MOCK_METHOD(void, set_selected_camera_sink, (uint32_t), (override));
            MOCK_METHOD(void, set_show_geometry, (bool), (override));
            MOCK_METHOD(void, set_show_triggers, (bool), (override));
            MOCK_METHOD(void, set_show_water, (bool), (override));
            MOCK_METHOD(void, set_show_wireframe, (bool), (override));
            MOCK_METHOD(void, set_show_bounding_boxes, (bool), (override));
            MOCK_METHOD(void, set_show_lighting, (bool), (override));
            MOCK_METHOD(void, set_show_lights, (bool), (override));
            MOCK_METHOD(void, set_show_items, (bool), (override));
            MOCK_METHOD(void, set_show_rooms, (bool), (override));
            MOCK_METHOD(void, set_neighbour_depth, (uint32_t), (override));
            MOCK_METHOD(void, set_selected_room, (const std::weak_ptr<IRoom>&), (override));
            MOCK_METHOD(void, set_selected_item, (const std::weak_ptr<IItem>&), (override));
            MOCK_METHOD(void, set_selected_flyby_node, (const std::weak_ptr<IFlybyNode>&), (override));
            MOCK_METHOD(bool, show_camera_sinks, (), (const, override));
            MOCK_METHOD(bool, show_lighting, (), (const, override));
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
            MOCK_METHOD(void, set_show_camera_sinks, (bool), (override));
            MOCK_METHOD(std::optional<uint32_t>, selected_camera_sink, (), (const, override));
            MOCK_METHOD(std::weak_ptr<IStaticMesh>, static_mesh, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<IStaticMesh>>, static_meshes, (), (const));
            MOCK_METHOD(std::weak_ptr<ISoundStorage>, sound_storage, (), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<ISoundSource>>, sound_sources, (), (const, override));
            MOCK_METHOD(void, set_show_sound_sources, (bool), (override));
            MOCK_METHOD(bool, show_sound_sources, (), (const, override));
            MOCK_METHOD(trlevel::Platform, platform, (), (const, override));
            MOCK_METHOD(trlevel::PlatformAndVersion, platform_and_version, (), (const, override));
            MOCK_METHOD(void, set_ng_plus, (bool), (override));
            MOCK_METHOD(bool, ng_plus, (), (const, override));
            MOCK_METHOD(bool, trng, (), (const, override));
            MOCK_METHOD(std::weak_ptr<trlevel::IPack>, pack, (), (const, override));
            MOCK_METHOD(std::string, hash, (), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<IFlyby>>, flybys, (), (const, override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(void, set_show_animation, (bool), (override));
            MOCK_METHOD(void, receive_message, (const Message&), (override));

            std::shared_ptr<MockLevel> with_version(trlevel::LevelVersion version)
            {
                ON_CALL(*this, version).WillByDefault(testing::Return(version));
                return shared_from_this();
            }
        };
    }
}
