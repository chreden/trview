#include <trview.app/UI/MapColours.h>

using namespace trview;

TEST(MapColours, OverrideColour)
{
    MapColours colours;
    ASSERT_EQ(colours.override_colours().size(), 0);
    auto previous = colours.colour(SectorFlag::Death);
    colours.set_colour(SectorFlag::Death, Colour::Blue);
    auto changed = colours.colour(SectorFlag::Death);
    ASSERT_NE(previous, changed);
    ASSERT_EQ(changed, Colour::Blue);
    ASSERT_EQ(colours.override_colours().size(), 1);
    ASSERT_EQ(colours.override_colours()[SectorFlag::Death], Colour::Blue);
    colours.clear_colour(SectorFlag::Death);
    auto cleared = colours.colour(SectorFlag::Death);
    ASSERT_EQ(cleared, previous);
}

TEST(MapColour, SpecialColour)
{
    MapColours colours;
    ASSERT_EQ(colours.special_colours().size(), 0);
    auto previous = colours.colour(MapColours::Special::Default);
    colours.set_colour(MapColours::Special::Default, Colour::Blue);
    auto changed = colours.colour(MapColours::Special::Default);
    ASSERT_NE(previous, changed);
    ASSERT_EQ(changed, Colour::Blue);
    ASSERT_EQ(colours.special_colours().size(), 1);
    ASSERT_EQ(colours.special_colours()[MapColours::Special::Default], Colour::Blue);
    colours.clear_colour(MapColours::Special::Default);
    auto cleared = colours.colour(MapColours::Special::Default);
    ASSERT_EQ(cleared, previous);
}

