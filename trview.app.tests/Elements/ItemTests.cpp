#include <trview.app/Elements/IItem.h>
#include <trview.app/Mocks/Elements/IItem.h>
#include <trview.tests.common/Mocks.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

TEST(Item, IsMutantEgg)
{
    ASSERT_TRUE(is_mutant_egg(*mock_shared<MockItem>()->with_type_id(163)));
    ASSERT_TRUE(is_mutant_egg(*mock_shared<MockItem>()->with_type_id(181)));
    ASSERT_FALSE(is_mutant_egg(*mock_shared<MockItem>()->with_type_id(0)));
}

TEST(Item, IsMutantEggId)
{
    ASSERT_TRUE(is_mutant_egg(163));
    ASSERT_TRUE(is_mutant_egg(181));
    ASSERT_FALSE(is_mutant_egg(0));
}

TEST(Item, MutantEggContents)
{
    ASSERT_EQ(20, mutant_egg_contents(*mock_shared<MockItem>()->with_type_id(163)->with_activation_flags(0)));
    ASSERT_EQ(21, mutant_egg_contents(*mock_shared<MockItem>()->with_type_id(163)->with_activation_flags(1)));
    ASSERT_EQ(23, mutant_egg_contents(*mock_shared<MockItem>()->with_type_id(163)->with_activation_flags(2)));
    ASSERT_EQ(34, mutant_egg_contents(*mock_shared<MockItem>()->with_type_id(163)->with_activation_flags(4)));
    ASSERT_EQ(22, mutant_egg_contents(*mock_shared<MockItem>()->with_type_id(163)->with_activation_flags(8)));
    ASSERT_EQ(20, mutant_egg_contents(*mock_shared<MockItem>()->with_type_id(163)->with_activation_flags(1851)));
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