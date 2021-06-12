#include <trview.app/Routing/Actions.h>

using namespace trview;
using namespace trlevel;

TEST(Actions, ActionsForLevelVersion)
{
    std::string json{ "{ \"actions\": [ { \"name\":\"test\", \"times\": { \"tr2\": 1.0 } }, { \"name\":\"test2\", \"times\": { \"tr2\": 1.0 } }, { \"name\":\"test3\", \"times\": { \"tr3\": 1.0 } } ] }" };
    Actions actions(json);

    auto result = actions.actions(LevelVersion::Tomb2);
    ASSERT_EQ(result.size(), 2);
}

TEST(Actions, ActionByNameReturnsValue)
{
    std::string json{ "{ \"actions\": [ { \"name\":\"test\", \"times\": { \"tr1\": 1.0 } } ] }" };
    Actions actions(json);

    auto result = actions.action("test");
    ASSERT_TRUE(result.has_value());
}

TEST(Actions, MissingActionByNameReturnsEmpty)
{
    std::string json{ "{ \"actions\": [] }" };
    Actions actions(json);

    auto result = actions.action("test");
    ASSERT_FALSE(result.has_value());
}
