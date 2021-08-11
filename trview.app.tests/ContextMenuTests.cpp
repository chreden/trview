#include <trview.app/UI/ContextMenu.h>

using namespace trview;
using namespace trview::tests;

TEST(ContextMenu, AddWaypointRaised)
{
    ui::Window parent(Size(800, 600), Colour::Transparent);
    ContextMenu menu(parent);
    menu.set_hide_enabled(true);

    bool raised = false;
    auto token = menu.on_add_waypoint += [&raised]() { raised = true; };

    auto button = parent.find<ui::Button>(ContextMenu::Names::add_waypoint_button);
    ASSERT_NE(button, nullptr);

    button->clicked(Point());
    ASSERT_TRUE(raised);
}

TEST(ContextMenu, HideButtonTextColourChangesWhenDisabled)
{
    ui::Window parent(Size(800, 600), Colour::Transparent);
    ContextMenu menu(parent);

    auto button = parent.find<ui::Button>(ContextMenu::Names::hide_button);
    ASSERT_NE(button, nullptr);
    auto current_colour = button->text_colour();
    ASSERT_TRUE(current_colour.has_value());

    menu.set_hide_enabled(true);
    auto new_colour = button->text_colour();
    ASSERT_TRUE(new_colour.has_value());

    ASSERT_NE(current_colour.value(), new_colour.value());
}

TEST(ContextMenu, HideNotRaisedWhenDisabled)
{
    ui::Window parent(Size(800, 600), Colour::Transparent);
    ContextMenu menu(parent);

    bool raised = false;
    auto token = menu.on_hide += [&raised]() { raised = true; };

    auto button = parent.find<ui::Button>(ContextMenu::Names::hide_button);
    ASSERT_NE(button, nullptr);

    button->clicked(Point());
    ASSERT_FALSE(raised);
}

TEST(ContextMenu, HideRaised)
{
    ui::Window parent(Size(800, 600), Colour::Transparent);
    ContextMenu menu(parent);
    menu.set_hide_enabled(true);

    bool raised = false;
    auto token = menu.on_hide += [&raised]() { raised = true; };

    auto button = parent.find<ui::Button>(ContextMenu::Names::hide_button);
    ASSERT_NE(button, nullptr);

    button->clicked(Point());
    ASSERT_TRUE(raised);
}

TEST(ContextMenu, OrbitHereRaised)
{
    ui::Window parent(Size(800, 600), Colour::Transparent);
    ContextMenu menu(parent);
    menu.set_hide_enabled(true);

    bool raised = false;
    auto token = menu.on_orbit_here += [&raised]() { raised = true; };

    auto button = parent.find<ui::Button>(ContextMenu::Names::orbit_button);
    ASSERT_NE(button, nullptr);

    button->clicked(Point());
    ASSERT_TRUE(raised);
}

TEST(ContextMenu, RemoveWaypointButtonTextColourChangesWhenDisabled)
{
    ui::Window parent(Size(800, 600), Colour::Transparent);
    ContextMenu menu(parent);

    auto button = parent.find<ui::Button>(ContextMenu::Names::remove_waypoint_button);
    ASSERT_NE(button, nullptr);
    auto current_colour = button->text_colour();
    ASSERT_TRUE(current_colour.has_value());

    menu.set_remove_enabled(true);
    auto new_colour = button->text_colour();
    ASSERT_TRUE(new_colour.has_value());

    ASSERT_NE(current_colour.value(), new_colour.value());
}

TEST(ContextMenu, RemoveWaypointNotRaisedWhenDisabled)
{
    ui::Window parent(Size(800, 600), Colour::Transparent);
    ContextMenu menu(parent);

    bool raised = false;
    auto token = menu.on_remove_waypoint += [&raised]() { raised = true; };

    auto button = parent.find<ui::Button>(ContextMenu::Names::remove_waypoint_button);
    ASSERT_NE(button, nullptr);

    button->clicked(Point());
    ASSERT_FALSE(raised);
}

TEST(ContextMenu, RemoveWaypointRaised)
{
    ui::Window parent(Size(800, 600), Colour::Transparent);
    ContextMenu menu(parent);
    menu.set_remove_enabled(true);

    bool raised = false;
    auto token = menu.on_remove_waypoint += [&raised]() { raised = true; };

    auto button = parent.find<ui::Button>(ContextMenu::Names::remove_waypoint_button);
    ASSERT_NE(button, nullptr);

    button->clicked(Point());
    ASSERT_TRUE(raised);
}

TEST(ContextMenu, ShowContextMenu)
{
    ui::Window parent(Size(800, 600), Colour::Transparent);
    ContextMenu menu(parent);

    auto control = menu.control();
    ASSERT_NE(control, nullptr);
    ASSERT_FALSE(control->visible());
    ASSERT_FALSE(menu.visible());
    
    menu.set_visible(true);
    ASSERT_TRUE(menu.visible());
    ASSERT_TRUE(control->visible());
}

TEST(ContextMenu, AddMidWaypointRaised)
{
    ui::Window parent(Size(800, 600), Colour::Transparent);
    ContextMenu menu(parent);
    menu.set_mid_waypoint_enabled(true);

    bool raised = false;
    auto token = menu.on_add_mid_waypoint += [&raised]() { raised = true; };

    auto button = parent.find<ui::Button>(ContextMenu::Names::add_mid_waypoint_button);
    ASSERT_NE(button, nullptr);

    button->clicked(Point());
    ASSERT_TRUE(raised);
}

TEST(ContextMenu, AddMidWaypointTextColourChangesWhenDisabled)
{
    ui::Window parent(Size(800, 600), Colour::Transparent);
    ContextMenu menu(parent);

    auto button = parent.find<ui::Button>(ContextMenu::Names::add_mid_waypoint_button);
    ASSERT_NE(button, nullptr);
    auto current_colour = button->text_colour();
    ASSERT_TRUE(current_colour.has_value());

    menu.set_mid_waypoint_enabled(true);
    auto new_colour = button->text_colour();
    ASSERT_TRUE(new_colour.has_value());

    ASSERT_NE(current_colour.value(), new_colour.value());
}

TEST(ContextMenu, AddMidWaypointNotRaisedWhenDisabled)
{
    ui::Window parent(Size(800, 600), Colour::Transparent);
    ContextMenu menu(parent);

    bool raised = false;
    auto token = menu.on_add_mid_waypoint += [&raised]() { raised = true; };

    auto button = parent.find<ui::Button>(ContextMenu::Names::add_mid_waypoint_button);
    ASSERT_NE(button, nullptr);

    button->clicked(Point());
    ASSERT_FALSE(raised);
}