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
    EXPECT_CALL(*bounding_mesh, render(A<const Matrix&>(), A<const ILevelTextureStorage&>(), A<const Color&>(), A<float>(), A<Vector3>(), A<bool>())).Times(1);

    StaticMesh mesh({}, {}, actual_mesh, bounding_mesh);
    mesh.render_bounding_box(NiceMock<MockCamera>{}, NiceMock<MockLevelTextureStorage>{}, Colour::White);
}
