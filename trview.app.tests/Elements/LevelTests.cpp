#include <trview.app/Elements/Level.h>
#include <trview.app/Elements/ITypeNameLookup.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.graphics/mocks/IDevice.h>
#include <trview.graphics/mocks/IShaderStorage.h>
#include <trview.app/Mocks/Geometry/ITransparencyBuffer.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.app/Mocks/Graphics/IMeshStorage.h>
#include <trview.app/Mocks/Graphics/ISelectionRenderer.h>
#include <trview.app/Mocks/Elements/ITypeNameLookup.h>
#include <trview.app/Mocks/Elements/IEntity.h>
#include <trview.app/Mocks/Elements/IRoom.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::graphics;
using namespace trview::graphics::mocks;
using namespace trlevel;
using namespace trlevel::mocks;
using testing::NiceMock;
using testing::Return;

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

    auto mock_type_name_lookup = std::make_shared<MockTypeNameLookup>();
    EXPECT_CALL(*mock_type_name_lookup, lookup_type_name(LevelVersion::Tomb2, 123));
    Level level(std::make_shared<MockDevice>(), std::make_shared<MockShaderStorage>(), std::move(mock_level),
        std::make_unique<MockLevelTextureStorage>(), std::make_unique<MockMeshStorage>(), std::make_unique<MockTransparencyBuffer>(),
        std::make_unique<MockSelectionRenderer>(), mock_type_name_lookup, [](auto, auto) { return std::make_unique<MockMesh>(); },
        [](auto&&, auto&&, auto&&, auto&&) { return std::make_shared<MockEntity>(); },
        [](auto&&, auto&&, auto&&, auto&) { return std::make_shared<MockEntity>(); }, 
        [](auto&&, auto&&, auto&&, auto&&, auto&&, auto&&) { return std::make_shared<MockRoom>(); });
}

TEST(Level, LoadFromEntitySources)
{
    tr2_entity entity;
    entity.Room = 0;
    entity.TypeID = 123;

    tr4_ai_object ai_object{};

    auto mock_level = std::make_unique<testing::NiceMock<MockLevel>>();
    EXPECT_CALL(*mock_level, get_version).WillRepeatedly(Return(LevelVersion::Tomb4));
    EXPECT_CALL(*mock_level, num_rooms()).WillRepeatedly(Return(1));
    EXPECT_CALL(*mock_level, num_entities()).WillRepeatedly(Return(1));
    EXPECT_CALL(*mock_level, get_entity(0)).WillRepeatedly(Return(entity));
    EXPECT_CALL(*mock_level, num_ai_objects()).WillRepeatedly(Return(1));
    EXPECT_CALL(*mock_level, get_ai_object(0)).WillRepeatedly(Return(ai_object));

    uint32_t entity_source_called = 0;
    uint32_t ai_source_called = 0;

    Level level(std::make_shared<MockDevice>(), std::make_shared<MockShaderStorage>(), std::move(mock_level),
        std::make_unique<MockLevelTextureStorage>(), std::make_unique<MockMeshStorage>(), std::make_unique<MockTransparencyBuffer>(),
        std::make_unique<MockSelectionRenderer>(), std::make_shared<MockTypeNameLookup>(), 
        [](auto, auto) { return std::make_unique<MockMesh>(); }, 
        [&](auto&&, auto&&, auto&&, auto&&) 
        { 
            ++entity_source_called;
            return std::make_shared<MockEntity>(); 
        },
        [&](auto&&, auto&&, auto&&, auto&) 
        { 
            ++ai_source_called;
            return std::make_shared<MockEntity>(); 
        },
        [](auto&&, auto&&, auto&&, auto&&, auto&&, auto&&) { return std::make_shared<MockRoom>(); });

    ASSERT_EQ(entity_source_called, 1);
    ASSERT_EQ(ai_source_called, 1);
}

TEST(Level, LoadRooms)
{
    auto mock_level = std::make_unique<testing::NiceMock<MockLevel>>();
    EXPECT_CALL(*mock_level, get_version).WillRepeatedly(Return(LevelVersion::Tomb4));
    EXPECT_CALL(*mock_level, num_rooms()).WillRepeatedly(Return(3));

    int room_called = 0;

    Level level(std::make_shared<MockDevice>(), std::make_shared<MockShaderStorage>(), std::move(mock_level),
        std::make_unique<MockLevelTextureStorage>(), std::make_unique<MockMeshStorage>(), std::make_unique<MockTransparencyBuffer>(),
        std::make_unique<MockSelectionRenderer>(), std::make_shared<MockTypeNameLookup>(),
        [](auto, auto) { return std::make_unique<MockMesh>(); },
        [](auto&&, auto&&, auto&&, auto&&) { return std::make_shared<MockEntity>(); },
        [](auto&&, auto&&, auto&&, auto&) { return std::make_shared<MockEntity>(); },
        [&](auto&&, auto&&, auto&&, auto&&, auto&&, auto&&) 
        { 
            ++room_called;
            return std::make_shared<MockRoom>(); 
        });

    ASSERT_EQ(room_called, 3);
}
