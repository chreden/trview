#include "gtest/gtest.h"
#include <trview.common/Algorithms.h>

using namespace trview;

TEST(equals_any, NotEqualsMany)
{
    int value = 15;
    bool result = trview::equals_any(value, 3, 10, 13);
    ASSERT_FALSE(result);
}

TEST(equals_any, EqualsMany)
{
    int value = 15;
    bool result = trview::equals_any(value, 4, 8, 15);
    ASSERT_TRUE(result);
}

TEST(equals_any, OneOtherEquals)
{
    int value = 15;
    bool result = trview::equals_any(value, 15);
    ASSERT_TRUE(result);
}

TEST(equals_any, OneOtherNotEquals)
{
    int value = 15;
    bool result = trview::equals_any(value, 16);
    ASSERT_FALSE(result);
}
