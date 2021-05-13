#include <trview.app/Elements/Room.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.app/Mocks/Graphics/IMeshStorage.h>
#include <trview.app/Mocks/Elements/IStaticMesh.h>
#include <trview.app/Mocks/Elements/ISector.h>
#include <external/boost/di.hpp>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace boost;
using testing::Return;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<ILevelTextureStorage> level_texture_storage{ std::make_shared<MockLevelTextureStorage>() };
            std::shared_ptr<IMeshStorage> mesh_storage{ std::make_shared<MockMeshStorage>() };
            IMesh::Source mesh_source{ [](auto&&...) { return std::make_shared<MockMesh>(); } };
            std::shared_ptr<trlevel::ILevel> tr_level{ std::make_shared<trlevel::mocks::MockLevel>() };
            trlevel::tr3_room room;
            uint32_t index{ 0u };
            std::shared_ptr<ILevel> level{ std::make_shared<MockLevel>() };
            IStaticMesh::MeshSource static_mesh_source{ [](auto&&...) { return std::make_shared<MockStaticMesh>(); } };
            IStaticMesh::PositionSource static_mesh_position_source{ [](auto&&...) { return std::make_shared<MockStaticMesh>(); } };
            ISector::Source sector_source{ [](auto&&...) { return std::make_shared<MockSector>(); } };

            std::unique_ptr<Room> build()
            {
                return di::make_injector
                (
                    di::bind<IMesh::Source>.to(mesh_source),
                    di::bind<trlevel::ILevel>.to(*tr_level),
                    di::bind<trlevel::tr3_room>.to(room),
                    di::bind<ILevelTextureStorage>.to(*level_texture_storage),
                    di::bind<IMeshStorage>.to(*mesh_storage),
                    di::bind<uint32_t>.to(index),
                    di::bind<ILevel>.to(*level),
                    di::bind<IStaticMesh::MeshSource>.to(static_mesh_source),
                    di::bind<IStaticMesh::PositionSource>.to(static_mesh_position_source),
                    di::bind<ISector::Source>.to(sector_source)
                ).create<std::unique_ptr<Room>>();
            }

            test_module& with_level(const std::shared_ptr<ILevel>& level)
            {
                this->level = level;
                return *this;
            }

            test_module& with_room(const trlevel::tr3_room& room)
            {
                this->room = room;
                return *this;
            }

            test_module& with_static_mesh_source(const IStaticMesh::MeshSource& static_mesh_source)
            {
                this->static_mesh_source = static_mesh_source;
                return *this;
            }

            test_module& with_static_mesh_position_source(const IStaticMesh::PositionSource& static_mesh_position_source)
            {
                this->static_mesh_position_source = static_mesh_position_source;
                return *this;
            }

            test_module& with_sector_source(const ISector::Source& sector_source)
            {
                this->sector_source = sector_source;
                return *this;
            }
        };
        return test_module{};
    }
}

/// <summary>
/// Tests that alternate group information is loaded from the level room.
/// </summary>
TEST(Room, AlternateGroupLoaded)
{
    trlevel::tr3_room level_room;
    level_room.alternate_group = 4;
    auto room = register_test_module().with_room(level_room).build();
    ASSERT_EQ(room->alternate_group(), 4);
}

/// <summary>
/// Tests that the room detects alternate room and has the correct alternate mode.
/// </summary>
TEST(Room, AlternateModeDetected)
{
    trlevel::tr3_room level_room;
    level_room.alternate_room = 100;
    auto room = register_test_module().with_room(level_room).build();
    ASSERT_EQ(room->alternate_mode(), IRoom::AlternateMode::HasAlternate);
    ASSERT_EQ(room->alternate_room(), 100);
}

/// <summary>
/// Tests that setting an alternate room marks this room as an alternate and returns the correct alternate room.
/// </summary>
TEST(Room, IsAlternateSet)
{
    auto room = register_test_module().build();
    ASSERT_EQ(room->alternate_mode(), IRoom::AlternateMode::None);
    room->set_is_alternate(100);
    ASSERT_EQ(room->alternate_mode(), IRoom::AlternateMode::IsAlternate);
    ASSERT_EQ(room->alternate_room(), 100);
}

/// <summary>
/// Tests that neighbours are correctly extracted from the sectors in the room.
/// </summary>
TEST(Room, NeighboursLoaded)
{
    trlevel::tr3_room level_room;
    level_room.sector_list.resize(2);

    std::set<uint16_t> expected{ 0, 10, 20, 30 };
    auto sector1 = std::make_shared<MockSector>();
    ON_CALL(*sector1, neighbours).WillByDefault(Return(std::set<uint16_t>{ 0, 20 }));
    auto sector2 = std::make_shared<MockSector>();
    ON_CALL(*sector2, neighbours).WillByDefault(Return(std::set<uint16_t>{ 0, 10, 30 }));
    uint32_t times_called = 0;

    auto source = [&](auto&&...)
    {
        if (times_called++)
        {
            return sector2;
        }
        return sector1;
    };

    auto room = register_test_module().with_room(level_room).with_sector_source(source).build();
    auto actual = room->neighbours();
    ASSERT_EQ(times_called, 2);
    ASSERT_EQ(actual, expected);
}

/// <summary>
/// Tests that the 'outside' flag is correctly detected.
/// </summary>
TEST(Room, OutsideDetected)
{
    trlevel::tr3_room level_room;
    level_room.flags |= 0x8;
    auto room = register_test_module().with_room(level_room).build();
    ASSERT_EQ(room->outside(), true);
}

/// <summary>
/// Tests that the 'quicksand' flag is correctly detected when the version is >= TR3.
/// </summary>
TEST(Room, QuicksandDetectedAfterTR3)
{
    trlevel::tr3_room level_room;
    level_room.flags |= 0x80;
    auto level = std::make_shared<MockLevel>();
    ON_CALL(*level, version).WillByDefault(Return(trlevel::LevelVersion::Tomb3));
    auto room = register_test_module().with_room(level_room).with_level(level).build();
    ASSERT_EQ(room->quicksand(), true);
}

/// <summary>
/// Tests that the 'quicksand' flag is not detected before TR3.
/// </summary>
TEST(Room, QuicksandNotDetectedBeforeTR3)
{
    trlevel::tr3_room level_room;
    level_room.flags |= 0x80;
    auto level = std::make_shared<MockLevel>();
    ON_CALL(*level, version).WillByDefault(Return(trlevel::LevelVersion::Tomb1));
    auto room = register_test_module().with_room(level_room).with_level(level).build();
    ASSERT_EQ(room->quicksand(), false);
}

/// <summary>
/// Tests that the appropriate number of static meshes in the room are created.
/// </summary>
TEST(Room, StaticMeshesLoaded)
{
    trlevel::tr3_room level_room;
    level_room.static_meshes.resize(2);

    uint32_t times_called = 0;
    auto static_mesh_source = [&](auto&&...)
    {
        ++times_called;
        return std::make_shared<MockStaticMesh>();
    };

    register_test_module().with_room(level_room).with_static_mesh_source(static_mesh_source).build();
    ASSERT_EQ(times_called, 2);
}

/// <summary>
/// Tests that the appropriate number of sprites in the room are created.
/// </summary>
TEST(Room, SpritesLoaded)
{
    trlevel::tr3_room level_room;
    level_room.data.vertices.resize(1);
    level_room.data.sprites.resize(2);

    uint32_t times_called = 0;
    auto static_mesh_position_source = [&](auto&&...)
    {
        ++times_called;
        return std::make_shared<MockStaticMesh>();
    };

    register_test_module().with_room(level_room).with_static_mesh_position_source(static_mesh_position_source).build();
    ASSERT_EQ(times_called, 2);
}

/// <summary>
/// Tests that the 'water' flag is correctly detected.
/// </summary>
TEST(Room, WaterDetected)
{
    trlevel::tr3_room level_room;
    level_room.flags |= 0x1;
    auto room = register_test_module().with_room(level_room).build();
    ASSERT_EQ(room->water(), true);
}
