#pragma once

#include <memory>

#include "../ILevel.h"

namespace trlevel
{
    namespace mocks
    {
        class MockLevel : public trlevel::ILevel
        {
        public:
            MockLevel();
            virtual ~MockLevel();
            MOCK_METHOD(tr_colour, get_palette_entry8, (uint32_t), (const, override));
            MOCK_METHOD(tr_colour4, get_palette_entry_16, (uint32_t), (const, override));
            MOCK_METHOD(tr_colour4, get_palette_entry, (uint32_t), (const, override));
            MOCK_METHOD(tr_colour4, get_palette_entry, (uint32_t, uint32_t), (const, override));
            MOCK_METHOD(uint32_t, num_rooms, (), (const, override));
            MOCK_METHOD(tr3_room, get_room, (uint32_t), (const, override));
            MOCK_METHOD(uint32_t, num_object_textures, (), (const, override));
            MOCK_METHOD(tr_object_texture, get_object_texture, (uint32_t), (const, override));
            MOCK_METHOD(uint32_t, num_floor_data, (), (const, override));
            MOCK_METHOD(uint16_t, get_floor_data, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<uint16_t>, get_floor_data_all, (), (const, override));
            MOCK_METHOD(uint32_t, num_ai_objects, (), (const, override));
            MOCK_METHOD(tr4_ai_object, get_ai_object, (uint32_t), (const, override));
            MOCK_METHOD(uint32_t, num_entities, (), (const, override));
            MOCK_METHOD(tr2_entity, get_entity, (uint32_t), (const, override));
            MOCK_METHOD(uint32_t, num_models, (), (const, override));
            MOCK_METHOD(tr_model, get_model, (uint32_t), (const, override));
            MOCK_METHOD(bool, get_model_by_id, (uint32_t, tr_model&), (const, override));
            MOCK_METHOD(uint32_t, num_static_meshes, (), (const, override));
            MOCK_METHOD(std::optional<tr_staticmesh>, get_static_mesh, (uint32_t), (const, override));
            MOCK_METHOD(uint32_t, num_mesh_pointers, (), (const, override));
            MOCK_METHOD(tr_mesh, get_mesh_by_pointer, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<tr_meshtree_node>, get_meshtree, (uint32_t, uint32_t), (const, override));
            MOCK_METHOD(tr2_frame, get_frame, (uint32_t, uint32_t), (const, override));
            MOCK_METHOD(LevelVersion, get_version, (), (const, override));
            MOCK_METHOD(bool, get_sprite_sequence_by_id, (int32_t, tr_sprite_sequence&), (const, override));
            MOCK_METHOD(std::optional<tr_sprite_texture>, get_sprite_texture, (uint32_t), (const, override));
            MOCK_METHOD(bool, find_first_entity_by_type, (int16_t, tr2_entity&), (const, override));
            MOCK_METHOD(std::string, name, (), (const, override));
            MOCK_METHOD(uint32_t, num_cameras, (), (const, override));
            MOCK_METHOD(tr_camera, get_camera, (uint32_t), (const, override));
            MOCK_METHOD(Platform, platform, (), (const, override));
            MOCK_METHOD(void, load, (const LoadCallbacks&), (override));
            MOCK_METHOD(std::vector<tr_sound_source>, sound_sources, (), (const, override));
            MOCK_METHOD(std::vector<tr_x_sound_details>, sound_details, (), (const, override));
            MOCK_METHOD(std::vector<int16_t>, sound_map, (), (const, override));
            MOCK_METHOD(bool, trng, (), (const, override));
            MOCK_METHOD(PlatformAndVersion, platform_and_version, (), (const, override));
            MOCK_METHOD(std::weak_ptr<IPack>, pack, (), (const, override));
        };
    }
}
