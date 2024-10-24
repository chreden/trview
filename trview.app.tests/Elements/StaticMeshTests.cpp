#include <trview.app/Elements/StaticMesh.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Camera/ICamera.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace DirectX::SimpleMath;
using testing::NiceMock;
using testing::A;

TEST(StaticMesh, BoundingBoxRendered)
{
    auto actual_mesh = mock_shared<MockMesh>();
    auto bounding_mesh = mock_shared<MockMesh>();
    EXPECT_CALL(*bounding_mesh, render(A<const Matrix&>(), A<const ILevelTextureStorage&>(), A<const Color&>(), A<float>(), A<Vector3>(), A<bool>(), A<bool>())).Times(1);

    StaticMesh mesh({}, {}, actual_mesh, {}, bounding_mesh);
    mesh.render_bounding_box(NiceMock<MockCamera>{}, NiceMock<MockLevelTextureStorage>{}, Colour::White);
}

TEST(StaticMesh, OnChangedRaised)
{
    StaticMesh mesh({}, {}, mock_shared<MockMesh>(), {}, mock_shared<MockMesh>());
    bool raised = false;
    auto token = mesh.on_changed += [&] (){ raised = true; };
    mesh.set_visible(false);
    ASSERT_EQ(raised, true);
}

TEST(StaticMesh, HasCollision)
{
    StaticMesh collision_mesh({}, { .Flags = 0 }, mock_shared<MockMesh>(), {}, mock_shared<MockMesh>());
    ASSERT_EQ(collision_mesh.has_collision(), true);
    StaticMesh no_collision_mesh({}, { .Flags = 1 }, mock_shared<MockMesh>(), {}, mock_shared<MockMesh>());
    ASSERT_EQ(no_collision_mesh.has_collision(), false);
}

TEST(StaticMesh, SpriteNoCollision)
{
    StaticMesh collision_mesh({}, {}, {}, mock_shared<MockMesh>(), {});
    ASSERT_EQ(collision_mesh.has_collision(), false);
}
