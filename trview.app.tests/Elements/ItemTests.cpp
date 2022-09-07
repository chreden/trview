#include <trview.app/Elements/Item.h>

using namespace trview;

TEST(Item, IsMutantEgg)
{
    ASSERT_TRUE(is_mutant_egg(Item(0, 0, 163, "", 0, 0, {}, {})));
    ASSERT_TRUE(is_mutant_egg(Item(0, 0, 181, "", 0, 0, {}, {})));
    ASSERT_FALSE(is_mutant_egg(Item(0, 0, 0, "", 0, 0, {}, {})));
}

TEST(Item, IsMutantEggId)
{
    ASSERT_TRUE(is_mutant_egg(163));
    ASSERT_TRUE(is_mutant_egg(181));
    ASSERT_FALSE(is_mutant_egg(0));
}

TEST(Item, MutantEggContents)
{
    ASSERT_EQ(20, mutant_egg_contents(Item(0, 0, 163, "", 0, 0 << 9, {}, {})));
    ASSERT_EQ(21, mutant_egg_contents(Item(0, 0, 163, "", 0, 1 << 9, {}, {})));
    ASSERT_EQ(23, mutant_egg_contents(Item(0, 0, 163, "", 0, 2 << 9, {}, {})));
    ASSERT_EQ(34, mutant_egg_contents(Item(0, 0, 163, "", 0, 4 << 9, {}, {})));
    ASSERT_EQ(22, mutant_egg_contents(Item(0, 0, 163, "", 0, 8 << 9, {}, {})));

    ASSERT_EQ(20, mutant_egg_contents(Item(0, 0, 163, "", 0, 1851, {}, {})));
}

TEST(Item, MutantEggContentsFlags)
{
    ASSERT_EQ(20, mutant_egg_contents(0));
    ASSERT_EQ(21, mutant_egg_contents(1));
    ASSERT_EQ(23, mutant_egg_contents(2));
    ASSERT_EQ(34, mutant_egg_contents(4));
    ASSERT_EQ(22, mutant_egg_contents(8));
    ASSERT_EQ(20, mutant_egg_contents(1851));
}