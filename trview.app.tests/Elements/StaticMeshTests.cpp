#include <trview.app/Elements/StaticMesh.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Camera/ICamera.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>

using namespace trview;
using namespace trview::mocks;
using namespace DirectX::SimpleMath;

TEST(StaticMesh, BoundingBoxRendered)
{
    auto actual_mesh = std::make_shared<MockMesh>();
    auto bounding_mesh = std::make_shared<MockMesh>();
    EXPECT_CALL(*bounding_mesh, render).Times(1);

    StaticMesh mesh({}, {}, actual_mesh, bounding_mesh);
    mesh.render_bounding_box(MockCamera{}, MockLevelTextureStorage{}, Colour::White);
}
