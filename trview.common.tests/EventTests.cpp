#include "gtest/gtest.h"
#include <string>
#include <trview.common/Event.h>

using namespace trview;

/// Test that chaining an event to another event works correctly.
TEST(Event, Chaining)
{
    int value = 0;
    int times_called = 0;

    Event<int> first, second;
    first += second;
    auto token = second += [&](int parameter)
    {
        ++times_called;
        value = parameter;
    };
    first(100);

    ASSERT_EQ(1, times_called);
    ASSERT_EQ(100, value);
}

/// Test that a function is called when the event is raised.
TEST(Event, FunctionCalledWhenEventRaised)
{
    int times_called = 0;
    int value = 0;

    Event<int> event;
    auto token = event += [&](auto v) {++times_called; value = v; };
    event(125);

    ASSERT_EQ(1, times_called);
    ASSERT_EQ(125, value);
}

TEST(Event, MultipleArgumentsPassed)
{
    int times_called = 0;
    int value_one = 0;
    std::string value_two;

    Event<int, std::string> event;
    auto token = event += [&](int v, std::string v2) { ++times_called; value_one = v; value_two = v2; };
    event(100, "Test");

    ASSERT_EQ(1, times_called);
    ASSERT_EQ(100, value_one);
    ASSERT_EQ(std::string("Test"), value_two);
}

TEST(Event, NoArgumentsPassed)
{
    int times_called = 0;

    Event<> event;
    auto token = event += [&]() {++times_called; };
    event();

    ASSERT_EQ(1, times_called);
}

TEST(Event, DeletedChainUpdatedCorrectly)
{
    int times_called = 0;
    Event<int> first, second;

    auto token = second += [&](auto) { ++times_called; };
    first += second;
    first(100);
    Event<int> third(std::move(second));
    first(100);

    ASSERT_EQ(2, times_called);
}

TEST(Event, DeletedChainAssignUpdatedCorrectly)
{
    int times_called = 0;
    int third_called = 0;
    Event<int> first, second;

    auto token = second += [&](auto) { ++times_called; };
    first += second;
    first(100);
    Event<int> third;
    auto token2 = third += [&](auto) { ++third_called; };
    third(100);
    ASSERT_EQ(1, third_called);

    third = std::move(second);
    first(100);

    ASSERT_EQ(1, third_called);
    ASSERT_EQ(2, times_called);
}

TEST(Event, TokenExpiryRemovesToken)
{
    int times_called = 0;
    Event<> event;
    {
        auto token = event += [&]() { ++times_called; };
        event();
        ASSERT_EQ(1, times_called);
    }
    event();
    ASSERT_EQ(1, times_called);
}
