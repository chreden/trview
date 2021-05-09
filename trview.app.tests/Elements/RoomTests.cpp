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
                    di::bind<IStaticMesh::PositionSource>.to(static_mesh_position_source)
                ).create<std::unique_ptr<Room>>();
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
        };
        return test_module{};
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

    register_test_module().with_room(level_room).with_static_mesh_source(static_mesh_source).build();
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

    register_test_module().with_room(level_room).with_static_mesh_position_source(static_mesh_position_source).build();
    ASSERT_EQ(times_called, 2);
}

