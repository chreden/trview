#include <gmock/gmock.h>
#include "gtest/gtest.h"
#include <trview.ui/Checkbox.h>
#include <trview.ui/Label.h>
#include <trview.ui/JsonLoader.h>
#include <trview.common/Mocks/Windows/IShell.h>
#include <optional>

using namespace trview;
using namespace trview::mocks;
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

TEST(Checkbox, LoadFromJson)
{
    const std::string json = "{ \"type\":\"checkbox\", \"text\":\"Test text\", \"state\":true }";

    auto loaded = JsonLoader(std::make_shared<MockShell>()).load_from_json(json);
    ASSERT_NE(loaded, nullptr);

    auto checkbox = dynamic_cast<Checkbox*>(loaded.get());
    ASSERT_NE(checkbox, nullptr);

    ASSERT_EQ(checkbox->text(), L"Test text");
    ASSERT_EQ(checkbox->state(), true);
}

TEST(Checkbox, Text)
{
    Checkbox checkbox(Point(), L"Test");
    ASSERT_EQ(checkbox.text(), L"Test");

    auto text = checkbox.find<Label>(Checkbox::Names::text);
    ASSERT_NE(text, nullptr);
    ASSERT_EQ(text->text(), L"Test");
}

TEST(Checkbox, Enabled)
{
    Checkbox checkbox;

    std::optional<bool> raised;
    auto token = checkbox.on_state_changed += [&](bool state)
    {
        raised = state;
    };

    ASSERT_TRUE(checkbox.enabled());
    checkbox.set_enabled(false);
    ASSERT_FALSE(checkbox.enabled());
    ASSERT_FALSE(checkbox.state());

    checkbox.clicked(Point());
    ASSERT_FALSE(raised.has_value());
    ASSERT_FALSE(checkbox.state());
}
