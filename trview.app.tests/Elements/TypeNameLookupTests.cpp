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

TEST(TypeNameLookup, LookupMutantEggs)
{
    FAIL();
}