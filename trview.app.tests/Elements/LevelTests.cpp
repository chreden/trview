#include "stdafx.h"
#include <trview.app/Elements/Level.h>
#include <trview.app/Elements/ITypeNameLookup.h>

using namespace trview;
using namespace trview::graphics;
using namespace trlevel;
using testing::NiceMock;
using testing::Return;

namespace
{
    class MockShaderStorage : public IShaderStorage
    {
    public:
        MOCK_METHOD2(add, void(const std::string&, std::unique_ptr<IShader>));
        MOCK_CONST_METHOD1(get, IShader*(const std::string&));
    };

    class MockLevel : public ILevel
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

    class MockTypeNameLookup : public ITypeNameLookup
    {
    public:
        MOCK_CONST_METHOD2(lookup_type_name, std::wstring(LevelVersion, uint32_t));
    };
}

// Tests that the level class loads the type names with the correct level version.
TEST(Level, LoadTypeNames)
{
    tr2_entity entity;
    entity.Room = 0;
    entity.TypeID = 123;

    auto mock_level = std::make_unique<testing::NiceMock<MockLevel>>();
    EXPECT_CALL(*mock_level, get_version)
        .WillRepeatedly(Return(LevelVersion::Tomb2));
    EXPECT_CALL(*mock_level, num_rooms())
        .WillRepeatedly(Return(1));
    EXPECT_CALL(*mock_level, num_entities())
        .WillRepeatedly(Return(1));
    EXPECT_CALL(*mock_level, get_entity(0))
        .WillRepeatedly(Return(entity));

    MockTypeNameLookup mock_type_name_lookup;
    EXPECT_CALL(mock_type_name_lookup, lookup_type_name(LevelVersion::Tomb2, 123));

    Level level(graphics::Device(), NiceMock<MockShaderStorage>(), std::move(mock_level), mock_type_name_lookup);
}