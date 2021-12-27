#include <trview.common/Size.h>
#include <trview.common/JsonSerializers.h>

using namespace trview;

TEST(Size, LoadFromJson)
{
    nlohmann::json json = nlohmann::json::parse("{ \"width\":200, \"height\":300 }");

    Size size;
    from_json(json, size);

    ASSERT_EQ(size.width, 200);
    ASSERT_EQ(size.height, 300);
}

