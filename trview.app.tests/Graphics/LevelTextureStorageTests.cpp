#include "stdafx.h"
#include <trview.app/Graphics/LevelTextureStorage.h>

using namespace trview;
using namespace trlevel;
using testing::Return;
using testing::_;
using testing::AtLeast;
using testing::Exactly;

namespace
{
    class MockLevel final : public ILevel
    {
    public:
        MOCK_CONST_METHOD1(get_palette_entry8, tr_colour(uint32_t));
        MOCK_CONST_METHOD1(get_palette_entry_16, tr_colour4(uint32_t));
        MOCK_CONST_METHOD1(get_palette_entry, tr_colour4(uint32_t));
        MOCK_CONST_METHOD2(get_palette_entry, tr_colour4(uint32_t, uint32_t));
        MOCK_CONST_METHOD0(num_textiles, uint32_t());
        MOCK_CONST_METHOD1(get_textile8, tr_textile8(uint32_t));
        MOCK_CONST_METHOD1(get_textile16, tr_textile16(uint32_t));
        MOCK_CONST_METHOD1(get_textile, std::vector<uint32_t>(uint32_t));
        MOCK_CONST_METHOD0(num_rooms, uint32_t());
        MOCK_CONST_METHOD1(get_room, tr3_room(uint32_t));
        MOCK_CONST_METHOD0(num_object_textures, uint32_t());
        MOCK_CONST_METHOD1(get_object_texture, tr_object_texture(uint32_t));
        MOCK_CONST_METHOD0(num_floor_data, uint32_t());
        MOCK_CONST_METHOD1(get_floor_data, uint16_t(uint32_t));
        MOCK_CONST_METHOD0(get_floor_data_all, std::vector<uint16_t>());
        MOCK_CONST_METHOD0(num_entities, uint32_t());
        MOCK_CONST_METHOD1(get_entity, tr2_entity(uint32_t));
        MOCK_CONST_METHOD0(num_models, uint32_t());
        MOCK_CONST_METHOD1(get_model, tr_model(uint32_t));
        MOCK_CONST_METHOD2(get_model_by_id, bool(uint32_t, tr_model&));
        MOCK_CONST_METHOD0(num_static_meshes, uint32_t());
        MOCK_CONST_METHOD1(get_static_mesh, tr_staticmesh(uint32_t));
        MOCK_CONST_METHOD0(num_mesh_pointers, uint32_t());
        MOCK_CONST_METHOD1(get_mesh_by_pointer, tr_mesh(uint32_t));
        MOCK_CONST_METHOD2(get_meshtree, std::vector<tr_meshtree_node>(uint32_t, uint32_t));
        MOCK_CONST_METHOD2(get_frame, tr2_frame(uint32_t, uint32_t));
        MOCK_CONST_METHOD0(get_version, LevelVersion());
        MOCK_CONST_METHOD2(get_sprite_sequence_by_id, bool(int32_t, tr_sprite_sequence&));
        MOCK_CONST_METHOD1(get_sprite_texture, tr_sprite_texture(uint32_t));
        MOCK_CONST_METHOD2(find_first_entity_by_type, bool(int16_t, tr2_entity&));
        MOCK_CONST_METHOD1(get_mesh_from_type_id, int16_t(int16_t));
    };
}

TEST(LevelTextureStorage, PaletteLoadedTomb1)
{
    MockLevel level;
    EXPECT_CALL(level, get_version()).WillRepeatedly(Return(LevelVersion::Tomb1));
    EXPECT_CALL(level, get_palette_entry(_)).Times(AtLeast(1));
    LevelTextureStorage subject(graphics::Device(), level);
}

TEST(LevelTextureStorage, PaletteLoadedTomb2)
{
    MockLevel level;
    EXPECT_CALL(level, get_version()).WillRepeatedly(Return(LevelVersion::Tomb2));
    EXPECT_CALL(level, get_palette_entry(_)).Times(AtLeast(1));
    LevelTextureStorage subject(graphics::Device(), level);
}

TEST(LevelTextureStorage, PaletteLoadedTomb3)
{
    MockLevel level;
    EXPECT_CALL(level, get_version()).WillRepeatedly(Return(LevelVersion::Tomb3));
    EXPECT_CALL(level, get_palette_entry(_)).Times(AtLeast(1));
    LevelTextureStorage subject(graphics::Device(), level);
}

TEST(LevelTextureStorage, PaletteNotLoadedTomb4)
{
    MockLevel level;
    EXPECT_CALL(level, get_version()).WillRepeatedly(Return(LevelVersion::Tomb4));
    EXPECT_CALL(level, get_palette_entry(_)).Times(Exactly(0));
    LevelTextureStorage subject(graphics::Device(), level);
}

TEST(LevelTextureStorage, PaletteNotLoadedTomb5)
{
    MockLevel level;
    EXPECT_CALL(level, get_version()).WillRepeatedly(Return(LevelVersion::Tomb5));
    EXPECT_CALL(level, get_palette_entry(_)).Times(Exactly(0));
    LevelTextureStorage subject(graphics::Device(), level);
}
