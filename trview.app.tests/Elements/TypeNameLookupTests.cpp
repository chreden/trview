#include "gtest/gtest.h"
#include <trview.app/Elements/TypeNameLookup.h>

using namespace trview;
using namespace trlevel;

TEST(TypeNameLookup, LookupTR1)
{
    std::string json = "{\"games\":{\"tr1\":[{\"id\":123,\"name\":\"Test Name\"}]}}";

    TypeNameLookup lookup(json);

    ASSERT_EQ(L"Test Name", lookup.lookup_type_name(LevelVersion::Tomb1, 123));
}

TEST(TypeNameLookup, LookupMultipleGames)
{
    std::string json = "{\"games\":{\"tr1\":[{\"id\":123,\"name\":\"Test Name TR1\"}]},\"tr2\":[{\"id\":123,\"name\":\"Test Name TR2\"}]}";

    TypeNameLookup lookup(json);

    ASSERT_EQ(L"Test Name TR1", lookup.lookup_type_name(LevelVersion::Tomb1, 123));
    ASSERT_EQ(L"Test Name TR2", lookup.lookup_type_name(LevelVersion::Tomb2, 123));
}
