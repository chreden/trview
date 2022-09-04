#include <trview.app/Elements/TypeNameLookup.h>

using namespace trview;
using namespace trlevel;

// Test that looking up an ID gives the correct result.
TEST(TypeNameLookup, LookupTR1)
{
    std::string json = "{\"games\":{\"tr1\":[{\"id\":123,\"name\":\"Test Name\"}]}}";

    TypeNameLookup lookup(json);

    ASSERT_EQ("Test Name", lookup.lookup_type_name(LevelVersion::Tomb1, 123, 0));
}

// Tests that if there are identical entries for different games, the correct result is returned.
TEST(TypeNameLookup, LookupMultipleGames)
{
    std::string json = "{\"games\":{\"tr1\":[{\"id\":123,\"name\":\"Test Name TR1\"}],\"tr2\":[{\"id\":123,\"name\":\"Test Name TR2\"}]}}";

    TypeNameLookup lookup(json);

    ASSERT_EQ("Test Name TR1", lookup.lookup_type_name(LevelVersion::Tomb1, 123, 0));
    ASSERT_EQ("Test Name TR2", lookup.lookup_type_name(LevelVersion::Tomb2, 123, 0));
}

// Tests that if the name is missing, it still returns the number.
TEST(TypeNameLookup, LookupMissingItem)
{
    std::string json = "{}";

    TypeNameLookup lookup(json);

    ASSERT_EQ("123", lookup.lookup_type_name(LevelVersion::Tomb3, 123, 0));
}

TEST(TypeNameLookup, LookupNormalMutantEggs)
{
    std::string json = "{\"games\":{\"tr1\":[{\"id\":163,\"name\":\"Test Name 1\"}]}}";
    TypeNameLookup lookup(json);

    auto winged = lookup.lookup_type_name(trlevel::LevelVersion::Tomb1, 163, 0);
    auto shooter = lookup.lookup_type_name(trlevel::LevelVersion::Tomb1, 163, 1 << 9);
    auto centaur = lookup.lookup_type_name(trlevel::LevelVersion::Tomb1, 163, 2 << 9);
    auto torso = lookup.lookup_type_name(trlevel::LevelVersion::Tomb1, 163, 4 << 9);
    auto grounded = lookup.lookup_type_name(trlevel::LevelVersion::Tomb1, 163, 8 << 9);
    auto def = lookup.lookup_type_name(trlevel::LevelVersion::Tomb1, 163, 3 << 9);

    ASSERT_EQ(winged, "Mutant Egg (Winged)");
    ASSERT_EQ(shooter, "Mutant Egg (Shooter)");
    ASSERT_EQ(centaur, "Mutant Egg (Centaur)");
    ASSERT_EQ(torso, "Mutant Egg (Torso)");
    ASSERT_EQ(grounded, "Mutant Egg (Grounded)");
    ASSERT_EQ(def, "Mutant Egg (Winged)");
}

TEST(TypeNameLookup, LookupBigMutantEggs)
{
    std::string json = "{\"games\":{\"tr1\":[{\"id\":181,\"name\":\"Test Name 2\"}]}}";
    TypeNameLookup lookup(json);

    auto winged = lookup.lookup_type_name(trlevel::LevelVersion::Tomb1, 181, 0);
    auto shooter = lookup.lookup_type_name(trlevel::LevelVersion::Tomb1, 181, 1 << 9);
    auto centaur = lookup.lookup_type_name(trlevel::LevelVersion::Tomb1, 181, 2 << 9);
    auto torso = lookup.lookup_type_name(trlevel::LevelVersion::Tomb1, 181, 4 << 9);
    auto grounded = lookup.lookup_type_name(trlevel::LevelVersion::Tomb1, 181, 8 << 9);
    auto def = lookup.lookup_type_name(trlevel::LevelVersion::Tomb1, 181, 3 << 9);

    ASSERT_EQ(winged, "Mutant Egg (Winged)");
    ASSERT_EQ(shooter, "Mutant Egg (Shooter)");
    ASSERT_EQ(centaur, "Mutant Egg (Centaur)");
    ASSERT_EQ(torso, "Mutant Egg (Torso)");
    ASSERT_EQ(grounded, "Mutant Egg (Grounded)");
    ASSERT_EQ(def, "Mutant Egg (Winged)");
}

TEST(TypeNameLookup, LookupNormalMutantEggsTR2)
{
    std::string json = "{\"games\":{\"tr1\":[{\"id\":163,\"name\":\"Test Name 1\"}],\"tr2\":[{\"id\":163,\"name\":\"Test Name 2\"}]}}";
    TypeNameLookup lookup(json);

    auto winged = lookup.lookup_type_name(trlevel::LevelVersion::Tomb2, 163, 0);

    ASSERT_EQ(winged, "Test Name 2");
}
