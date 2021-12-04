#include "gtest/gtest.h"
#include <trview.common/Colour.h>
#include <trview.common/JsonSerializers.h>

using namespace trview;

TEST(Colour, LoadNamedColourFromJson)
{
    nlohmann::json json = nlohmann::json::parse("\"red\"");

    Colour colour;
    from_json(json, colour);

    ASSERT_EQ(colour, Colour::Red);
}

TEST(Colour, LoadRGBFromJson)
{
    nlohmann::json json = nlohmann::json::parse("{\"r\":0.2,\"g\":0.4,\"b\":0.6}");
    Colour colour;
    from_json(json, colour);

    ASSERT_EQ(colour, Colour(1.0f, 0.2f, 0.4f, 0.6f));
}

TEST(Colour, LoadARGBFromJson)
{
    nlohmann::json json = nlohmann::json::parse("{\"r\":0.2,\"g\":0.4,\"b\":0.6,\"a\":0.8}");
    Colour colour;
    from_json(json, colour);

    ASSERT_EQ(colour, Colour(0.8f, 0.2f, 0.4f, 0.6f));
}
