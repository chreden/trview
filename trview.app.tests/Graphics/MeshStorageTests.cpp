#include <trview.app/Graphics/MeshStorage.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trlevel/Mocks/ILevel.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace trlevel;
using namespace trlevel::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            IMesh::Source mesh_source{ [](auto&&...) { return mock_shared<MockMesh>(); } };
            std::shared_ptr<trlevel::ILevel> level{ std::make_shared<trlevel::mocks::MockLevel>() };
            std::shared_ptr<ILevelTextureStorage> texture_storage{ mock_shared<MockLevelTextureStorage>() };

            std::unique_ptr<MeshStorage> build()
            {
                return std::make_unique<MeshStorage>(mesh_source, *level, *texture_storage);
            }

            test_module& with_level(const std::shared_ptr<trlevel::ILevel>& level)
            {
                this->level = level;
                return *this;
            }
        };
        return test_module{};
    }
}

TEST(MeshStorage, MeshesLoadedFromLevel)
{
    auto level = std::make_shared<trlevel::mocks::MockLevel>();
    ON_CALL(*level, num_mesh_pointers).WillByDefault(testing::Return(2));
    EXPECT_CALL(*level, get_mesh_by_pointer(0)).Times(1);
    EXPECT_CALL(*level, get_mesh_by_pointer(1)).Times(1);
    auto storage = register_test_module().with_level(level).build();
}

TEST(MeshStorage, MeshCanBeRetrieved)
{
    auto level = std::make_shared<trlevel::mocks::MockLevel>();
    ON_CALL(*level, num_mesh_pointers).WillByDefault(testing::Return(1));
    EXPECT_CALL(*level, get_mesh_by_pointer(0)).Times(1);
    auto storage = register_test_module().with_level(level).build();
    auto mesh = storage->mesh(0);
    ASSERT_NE(mesh, nullptr);
}

TEST(MeshStorage, MissingMeshNotFound)
{
    auto level = std::make_shared<trlevel::mocks::MockLevel>();
    ON_CALL(*level, num_mesh_pointers).WillByDefault(testing::Return(1));
    EXPECT_CALL(*level, get_mesh_by_pointer(0)).Times(1);
    auto storage = register_test_module().with_level(level).build();
    auto mesh = storage->mesh(1);
    ASSERT_EQ(mesh, nullptr);
}
