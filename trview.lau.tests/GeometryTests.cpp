#include <trview.lau/Geometry.h>

using namespace trview;
using namespace trview::lau;
using namespace DirectX::SimpleMath;

TEST(Drm, ConvertUV)
{
    const int32_t initial_value = -1088078461;
    Vector2 result = convert_uv(initial_value);
    ASSERT_EQ(result.x, 0.0639648438f);
    ASSERT_EQ(result.y, -0.644531250f);
    const int32_t recreated = make_uv(result);
    ASSERT_EQ(recreated, -1088078461);
}

