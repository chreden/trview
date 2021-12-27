#include <trview.ui/Control.h>
#include <optional>

using namespace trview;
using namespace trview::ui;

namespace
{
    class TestControl final : public Control
    {
    public:
        TestControl() : Control(Point(), Size())
        {

        }

        TestControl(Point position, Size size) : Control(position, size)
        {
        }
        virtual ~TestControl() = default;
    };
}

TEST(Control, PositionAndSize)
{
    TestControl control(Point(10, 20), Size(20, 30));
    ASSERT_EQ(control.position(), Point(10, 20));
    ASSERT_EQ(control.size(), Size(20, 30));
}

TEST(Control, AbsolutePosition)
{
    TestControl parent(Point(10, 20), Size(10, 10));
    auto control = parent.add_child(std::make_unique<TestControl>(Point(10, 20), Size(20, 30)));
    ASSERT_EQ(control->absolute_position(), Point(20, 40));
}

TEST(Control, VisibilityInTree)
{
    TestControl parent;
    auto control = parent.add_child(std::make_unique<TestControl>());
    
    ASSERT_TRUE(control->visible());
    ASSERT_TRUE(control->visible(true));

    parent.set_visible(false);
    ASSERT_TRUE(control->visible());
    ASSERT_FALSE(control->visible(true));
}

TEST(Control, AddChild)
{
    TestControl control;
    auto child = std::make_unique<TestControl>();
    child->set_name("Child");
    auto child_ptr = child.get();
    auto added = control.add_child(std::move(child));

    ASSERT_NE(added, nullptr);
    ASSERT_EQ(added, child_ptr);
    ASSERT_EQ(control.child_elements(), std::vector<Control*>{ child_ptr });
}

TEST(Control, Find)
{
    TestControl control;

    auto child = std::make_unique<TestControl>();
    child->set_name("Child");
    auto child_ptr = child.get();
    auto added = control.add_child(std::move(child));

    auto found = control.find<Control>("Child");

    ASSERT_EQ(added, child_ptr);
    ASSERT_EQ(found, child_ptr);
}

TEST(Control, FindType)
{
    TestControl control;

    auto child = std::make_unique<TestControl>();
    child->set_name("Child");
    auto child_ptr = child.get();
    auto added = control.add_child(std::move(child));

    auto found = control.find<TestControl>();

    const std::vector<TestControl*> expected{ &control, child_ptr };
    ASSERT_EQ(added, child_ptr);
    ASSERT_THAT(found, testing::ContainerEq(expected));
}

TEST(Control, RemoveChild)
{
    TestControl control;
    auto child = std::make_unique<TestControl>();
    auto added = control.add_child(std::move(child));
    auto removed = control.remove_child(added);
    ASSERT_EQ(removed.get(), added);
    ASSERT_EQ(control.child_elements(), std::vector<Control*>{ });
}

TEST(Control, AddChildRaised)
{
    TestControl control;

    auto child = std::make_unique<TestControl>();
    auto child_ptr = child.get();

    std::optional<Control*> raised;
    auto token = control.on_add_child += [&](Control* value)
    {
        raised = value;
    };

    auto added = control.add_child(std::move(child));
    ASSERT_NE(added, nullptr);
    ASSERT_EQ(added, child_ptr);
    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), child_ptr);
}

TEST(Control, RemoveChildRaised)
{
    TestControl control;

    std::optional<Control*> raised;
    auto token = control.on_remove_child += [&](const auto& value)
    {
        raised = value;
    };

    auto child = std::make_unique<TestControl>();
    auto added = control.add_child(std::move(child));
    auto removed = control.remove_child(added);

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), added);
}

TEST(Control, SizeChangedRaised)
{
    TestControl control;

    std::optional<Size> raised;
    auto token = control.on_size_changed += [&](const auto& value)
    {
        raised = value;
    };

    control.set_size(Size(100, 200));

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), Size(100, 200));
    ASSERT_EQ(control.size(), Size(100, 200));
}

TEST(Control, InvalidateRaised)
{
    TestControl control;

    bool raised = false;
    auto token = control.on_invalidate += [&]()
    {
        raised = true;
    };

    control.set_size(Size(100, 200));
    ASSERT_TRUE(raised);
}

TEST(Control, HierarchyChangedRaised)
{
    TestControl control;
    bool raised = false;
    auto token = control.on_hierarchy_changed += [&]()
    {
        raised = true;
    };

    control.add_child(std::make_unique<TestControl>());
    ASSERT_TRUE(raised);
}

TEST(Control, FocusLostRaised)
{
    TestControl control;
    bool raised = false;
    auto token = control.on_focus_lost += [&]()
    {
        raised = true;
    };
    control.lost_focus(nullptr);
    ASSERT_TRUE(raised);
}

TEST(Control, DeletingRaised)
{
    bool raised = false;
    Event<>::Token token(nullptr);
    {
        TestControl control;
        token = control.on_deleting += [&]()
        {
            raised = true;
        };
    }
    ASSERT_TRUE(raised);
}

TEST(Control, FocusedRaised)
{
    TestControl control;
    bool raised = false;
    auto token = control.on_focused += [&]()
    {
        raised = true;
    };
    control.gained_focus();
    ASSERT_TRUE(raised);
}

TEST(Control, UpdateRaised)
{
    TestControl control;

    std::optional<float> raised;
    auto token = control.on_update += [&](float value)
    {
        raised = value;
    };

    control.update(1.0f);

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 1.0f);
}

TEST(Control, UpdateChild)
{
    TestControl control;
    auto child = control.add_child(std::make_unique<TestControl>());

    std::optional<float> raised;
    auto token = child->on_update += [&](float value)
    {
        raised = value;
    };

    control.update(1.0f);

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 1.0f);
}
