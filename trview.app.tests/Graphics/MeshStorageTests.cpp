#include <trview.app/Graphics/MeshStorage.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trlevel/Mocks/ILevel.h>
#include <external/boost/di.hpp>

using namespace trview;
using namespace trview::mocks;
using namespace trlevel;
using namespace trlevel::mocks;

namespace
{
    auto register_test_module(ILevel& level)
    {
        using namespace boost;
        return di::make_injector
        (
            di::bind<IMesh::Source>.to([](auto&&) { return [](auto&&...) { return std::make_shared<MockMesh>(); }; }),
            di::bind<ILevel>.to(level),
            di::bind<ILevelTextureStorage>.to<MockLevelTextureStorage>()
        ).create<std::unique_ptr<MeshStorage>>();
    }
}

TEST(MeshStorage, MeshesLoadedFromLevel)
{
    MockLevel level;
    ON_CALL(level, num_mesh_pointers).WillByDefault(testing::Return(2));
    EXPECT_CALL(level, get_mesh_by_pointer(0)).Times(1);
    EXPECT_CALL(level, get_mesh_by_pointer(1)).Times(1);
    auto storage = register_test_module(level);
}

TEST(MeshStorage, MeshCanBeRetrieved)
{
    MockLevel level;
    ON_CALL(level, num_mesh_pointers).WillByDefault(testing::Return(1));
    EXPECT_CALL(level, get_mesh_by_pointer(0)).Times(1);
    auto storage = register_test_module(level);
    auto mesh = storage->mesh(0);
    ASSERT_NE(mesh, nullptr);
}

TEST(MeshStorage, MissingMeshNotFound)
{
    MockLevel level;
    ON_CALL(level, num_mesh_pointers).WillByDefault(testing::Return(1));
    EXPECT_CALL(level, get_mesh_by_pointer(0)).Times(1);
    auto storage = register_test_module(level);
    auto mesh = storage->mesh(1);
    ASSERT_EQ(mesh, nullptr);
}
