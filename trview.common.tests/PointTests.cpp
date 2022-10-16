#include <trview.common/Point.h>
#include <trview.common/JsonSerializers.h>

using namespace trview;

TEST(Point, LoadFromJson)
{
    nlohmann::json json = nlohmann::json::parse("{ \"x\":200, \"y\":300 }");

    Point point;
    from_json(json, point);

    ASSERT_EQ(point.x, 200);
    ASSERT_EQ(point.y, 300);
}

