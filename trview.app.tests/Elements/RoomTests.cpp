#include <trview.app/Elements/Room.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.app/Mocks/Graphics/IMeshStorage.h>
#include <trview.app/Mocks/Elements/IStaticMesh.h>
#include <external/boost/di.hpp>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace boost;

namespace
{
    auto default_mesh_source = [](auto&&...) { return std::make_shared<MockMesh>(); };
    auto default_static_mesh_source = [](auto&&...) { return std::make_shared<MockStaticMesh>(); };
    auto default_static_mesh_position_source = [](auto&&...) { return std::make_shared<MockStaticMesh>(); };

    auto register_test_module(IMesh::Source mesh_source = default_mesh_source, std::shared_ptr<trlevel::ILevel> tr_level = nullptr, trlevel::tr3_room room = {},
        std::shared_ptr<ILevelTextureStorage> level_texture_storage = nullptr, std::shared_ptr<IMeshStorage> mesh_storage = nullptr, uint32_t index = 0,
        std::shared_ptr<ILevel> level = nullptr, IStaticMesh::MeshSource static_mesh_source = default_static_mesh_source,
        IStaticMesh::PositionSource static_mesh_position_source = default_static_mesh_position_source)
    {
        choose_mock<trlevel::mocks::MockLevel>(tr_level);
        choose_mock<MockLevelTextureStorage>(level_texture_storage);
        choose_mock<MockMeshStorage>(mesh_storage);
        choose_mock<MockLevel>(level);

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
            di::bind<IStaticMesh::PositionSource>.to(static_mesh_position_source)
        ).create<std::unique_ptr<Room>>();
    }
}

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

    auto room = register_test_module(default_mesh_source, nullptr, level_room, nullptr, nullptr, 0, nullptr, static_mesh_source, default_static_mesh_position_source);
    ASSERT_EQ(times_called, 2);
}

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

    auto room = register_test_module(default_mesh_source, nullptr, level_room, nullptr, nullptr, 0, nullptr, default_static_mesh_source, static_mesh_position_source);
    ASSERT_EQ(times_called, 2);
}

