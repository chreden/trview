#pragma once

#include "../ILevel.h"

namespace trlevel
{
    namespace mocks
    {
        class MockLevel : public trlevel::ILevel
        {
        public:
            MOCK_METHOD(tr_colour, get_palette_entry8, (uint32_t), (const, override));
            MOCK_METHOD(tr_colour4, get_palette_entry_16, (uint32_t), (const, override));
            MOCK_METHOD(tr_colour4, get_palette_entry, (uint32_t), (const, override));
            MOCK_METHOD(tr_colour4, get_palette_entry, (uint32_t, uint32_t), (const, override));
            MOCK_METHOD(uint32_t, num_textiles, (), (const, override));
            MOCK_METHOD(tr_textile8, get_textile8, (uint32_t), (const, override));
            MOCK_METHOD(tr_textile16, get_textile16, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<uint32_t>, get_textile, (uint32_t), (const, override));
            MOCK_METHOD(uint32_t, num_rooms, (), (const, override));
            MOCK_METHOD(tr3_room, get_room, (uint32_t), (const, override));
            MOCK_METHOD(uint32_t, num_object_textures, (), (const, override));
            MOCK_METHOD(tr_object_texture, get_object_texture, (uint32_t), (const, override));
            MOCK_METHOD(uint32_t, num_floor_data, (), (const, override));
            MOCK_METHOD(uint16_t, get_floor_data, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<uint16_t>, get_floor_data_all, (), (const, override));
            MOCK_METHOD(uint32_t, num_entities, (), (const, override));
            MOCK_METHOD(tr2_entity, get_entity, (uint32_t), (const, override));
            MOCK_METHOD(uint32_t, num_models, (), (const, override));
            MOCK_METHOD(tr_model, get_model, (uint32_t), (const, override));
            MOCK_METHOD(bool, get_model_by_id, (uint32_t, tr_model&), (const, override));
            MOCK_METHOD(uint32_t, num_static_meshes, (), (const, override));
            MOCK_METHOD(tr_staticmesh, get_static_mesh, (uint32_t), (const, override));
            MOCK_METHOD(uint32_t, num_mesh_pointers, (), (const, override));
            MOCK_METHOD(tr_mesh, get_mesh_by_pointer, (uint32_t), (const, override));
            MOCK_METHOD(std::vector<tr_meshtree_node>, get_meshtree, (uint32_t, uint32_t), (const, override));
            MOCK_METHOD(tr2_frame, get_frame, (uint32_t, uint32_t), (const, override));
            MOCK_METHOD(LevelVersion, get_version, (), (const, override));
            MOCK_METHOD(bool, get_sprite_sequence_by_id, (int32_t, tr_sprite_sequence&), (const, override));
            MOCK_METHOD(tr_sprite_texture, get_sprite_texture, (uint32_t), (const, override));
            MOCK_METHOD(bool, find_first_entity_by_type, (int16_t, tr2_entity&), (const, override));
            MOCK_METHOD(int16_t, get_mesh_from_type_id, (int16_t), (const, override));
        };
    }
}
