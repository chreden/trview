#include <trview.app/Routing/Action.h>

using namespace trview;
using namespace trlevel;

TEST(Action, AvailableForLevel)
{
    Action action("test", "test",
        {
            { LevelVersion::Tomb2, 1.0f }
        });
    ASSERT_TRUE(action.is_available(LevelVersion::Tomb2));
    ASSERT_FALSE(action.is_available(LevelVersion::Tomb1));
}

TEST(Action, TimeForLevel)
{
    Action action("test", "test", 
        {
            { LevelVersion::Tomb2, 1.0f },
            { LevelVersion::Tomb3, 2.5f }
        });
    ASSERT_EQ(action.time(LevelVersion::Tomb2), 1.0f);
    ASSERT_EQ(action.time(LevelVersion::Tomb3), 2.5f);
}

TEST(Action, Name)
{
    Action action("test_name", "test_display_name", {});
    ASSERT_EQ(action.name(), "test_name");
}

TEST(Action, DisplayName)
{
    Action action("test_name", "test_display_name", {});
    ASSERT_EQ(action.display_name(), "test_display_name");
}
