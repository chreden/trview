#include <trview.app/Elements/Level.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.graphics/mocks/IDevice.h>
#include <trview.graphics/mocks/IShaderStorage.h>
#include <trview.app/Mocks/Geometry/ITransparencyBuffer.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.app/Mocks/Graphics/IMeshStorage.h>
#include <trview.app/Mocks/Graphics/ISelectionRenderer.h>
#include <trview.app/Mocks/Elements/ITypeNameLookup.h>
#include <trview.app/Mocks/Elements/IEntity.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <external/boost/di.hpp>

using namespace trview;
using namespace trview::mocks;
using namespace trview::graphics;
using namespace trview::graphics::mocks;
using namespace trlevel;
using namespace trlevel::mocks;
using namespace boost;
using namespace trview::tests;
using testing::Return;

namespace
{
    auto default_entity_source = [](auto&&...) { return std::make_shared<MockEntity>(); };
    auto default_ai_source = [](auto&&...) { return std::make_shared<MockEntity>(); };
    auto default_room_source = [](auto&&...) { return std::make_shared<MockRoom>(); };

    auto register_test_module(std::unique_ptr<trlevel::ILevel> level, std::shared_ptr<ITypeNameLookup> type_name_lookup = nullptr, IEntity::EntitySource entity_source = default_entity_source,
        IEntity::AiSource ai_source = default_ai_source, IRoom::Source room_source = default_room_source)
    {
        choose_mock<trlevel::mocks::MockLevel>(level);
        choose_mock<MockTypeNameLookup>(type_name_lookup);

        return di::make_injector
        (
            di::bind<IDevice>.to<MockDevice>(),
            di::bind<IShaderStorage>.to<MockShaderStorage>(),
            di::bind<ITransparencyBuffer>.to<MockTransparencyBuffer>(),
            di::bind<ISelectionRenderer>.to<MockSelectionRenderer>(),
            di::bind<ITypeNameLookup>.to(type_name_lookup),
            di::bind<ILevelTextureStorage>.to<MockLevelTextureStorage>(),
            di::bind<trlevel::ILevel>.to([&](auto&&) { return std::move(level); }),
            di::bind<IMeshStorage>.to<MockMeshStorage>(),
            di::bind<IEntity::EntitySource>.to(entity_source),
            di::bind<IEntity::AiSource>.to(ai_source),
            di::bind<IRoom::Source>.to(room_source),
            di::bind<Level>()
        ).create<std::unique_ptr<Level>>();
    }
}

// Tests that the level class loads the type names with the correct level version.
TEST(Level, LoadTypeNames)
{
    tr2_entity entity;
    entity.Room = 0;
    entity.TypeID = 123;

    auto [mock_level_ptr, mock_level] = create_mock<MockLevel>();
    EXPECT_CALL(mock_level, get_version).WillRepeatedly(Return(LevelVersion::Tomb2));
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_level, num_entities()).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_level, get_entity(0)).WillRepeatedly(Return(entity));

    auto mock_type_name_lookup = std::make_shared<MockTypeNameLookup>();
    EXPECT_CALL(*mock_type_name_lookup, lookup_type_name(LevelVersion::Tomb2, 123));
    auto level = register_test_module(std::move(mock_level_ptr), mock_type_name_lookup);
}

TEST(Level, LoadFromEntitySources)
{
    tr2_entity entity;
    entity.Room = 0;
    entity.TypeID = 123;

    tr4_ai_object ai_object{};

    auto [mock_level_ptr, mock_level] = create_mock<MockLevel>();
    EXPECT_CALL(mock_level, get_version).WillRepeatedly(Return(LevelVersion::Tomb4));
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_level, num_entities()).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_level, get_entity(0)).WillRepeatedly(Return(entity));
    EXPECT_CALL(mock_level, num_ai_objects()).WillRepeatedly(Return(1));
    EXPECT_CALL(mock_level, get_ai_object(0)).WillRepeatedly(Return(ai_object));

    uint32_t entity_source_called = 0;
    uint32_t ai_source_called = 0;

    auto level = register_test_module(std::move(mock_level_ptr), nullptr,
        [&](auto&&...)
        {
            ++entity_source_called;
            return std::make_shared<MockEntity>();
        },
        [&](auto&&...)
        {
            ++ai_source_called;
            return std::make_shared<MockEntity>();
        });

    ASSERT_EQ(entity_source_called, 1);
    ASSERT_EQ(ai_source_called, 1);
}

TEST(Level, LoadRooms)
{
    auto [mock_level_ptr, mock_level] = create_mock<MockLevel>();
    EXPECT_CALL(mock_level, get_version).WillRepeatedly(Return(LevelVersion::Tomb4));
    EXPECT_CALL(mock_level, num_rooms()).WillRepeatedly(Return(3));

    int room_called = 0;

    auto level = register_test_module(std::move(mock_level_ptr), nullptr, default_entity_source, default_ai_source, 
        [&](auto&&...) 
        { 
            ++room_called;
            return std::make_shared<MockRoom>(); 
        });

    ASSERT_EQ(room_called, 3);
}
