#include "gtest/gtest.h"
#include <trview.ui/Checkbox.h>

using namespace trview;
using namespace trview::ui;

/// Tests that the state change event is not raised when the set_state function
/// is called rather than the user clicking on the control.
TEST(Checkbox, StateChangeEventNotRaised)
{
    Checkbox checkbox;
    bool raised = false;
    auto token = checkbox.on_state_changed += [&](bool)
    {
        raised = true;
    };

    ASSERT_FALSE(checkbox.state());
    checkbox.set_state(true);
    ASSERT_FALSE(raised);
    ASSERT_TRUE(checkbox.state());
}

/// Tests that the state change event is raised when the user clicks on the control.
TEST(Checkbox, StateChangeEventRaised)
{
    Checkbox checkbox;

    bool raised = false;
    uint32_t times = 0;
    bool raised_state = false;
    auto token = checkbox.on_state_changed += [&raised, &times, &raised_state](bool state)
    {
        raised = true;
        raised_state = state;
        ++times;
    };

    checkbox.clicked(Point());
    ASSERT_TRUE(raised);
    ASSERT_EQ(1u, times);
    ASSERT_EQ(true, raised_state);
    ASSERT_EQ(raised_state, checkbox.state());
}

/// Tests that the state of the checkbox can cycle between checked and unchecked and raises
/// an event each time the state changes.
TEST(Checkbox, StateChangeCycle)
{
    Checkbox checkbox;

    std::vector<bool> states;
    auto token = checkbox.on_state_changed += [&states](bool state)
    {
        states.push_back(state);
    };

    checkbox.clicked(Point());
    checkbox.clicked(Point());
    ASSERT_EQ((std::size_t)2u, states.size());
    ASSERT_EQ(true, static_cast<bool>(states[0]));
    ASSERT_EQ(false, static_cast<bool>(states[1]));
}
