#include <trview.app/UI/ContextMenu.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;

TEST(ContextMenu, AddWaypointRaised)
{
    ContextMenu menu;
    menu.set_visible(true);

    TestImgui imgui([&]() { menu.render(); });

    bool raised = false;
    auto token = menu.on_add_waypoint += [&raised]() { raised = true; };

    imgui.show_context_menu("Debug##Default");
    imgui.click_element(imgui.popup_id("void_context").id(ContextMenu::Names::add_waypoint), true);
    imgui.render();

    ASSERT_TRUE(raised);
    ASSERT_FALSE(menu.visible());
}

TEST(ContextMenu, HideButtonDisabled)
{
    ContextMenu menu;
    menu.set_visible(true);
    menu.set_hide_enabled(true);

    TestImgui imgui([&]() { menu.render(); });

    imgui.show_context_menu("Debug##Default");
    const auto original_flags = imgui.item_flags(imgui.popup_id("void_context").id(ContextMenu::Names::hide));
    ASSERT_FALSE(original_flags & ImGuiItemFlags_Disabled);

    menu.set_hide_enabled(false);
    imgui.render();

    const auto new_flags = imgui.item_flags(imgui.popup_id("void_context").id(ContextMenu::Names::hide));
    ASSERT_TRUE(new_flags & ImGuiItemFlags_Disabled);
}

TEST(ContextMenu, HideNotRaisedWhenDisabled)
{
    ContextMenu menu;
    menu.set_visible(true);
    menu.set_hide_enabled(false);

    TestImgui imgui([&]() { menu.render(); });

    bool raised = false;
    auto token = menu.on_hide += [&raised]() { raised = true; };

    imgui.show_context_menu("Debug##Default");
    imgui.click_element(imgui.popup_id("void_context").id(ContextMenu::Names::hide), true);

    ASSERT_FALSE(raised);
    ASSERT_TRUE(menu.visible());
}

TEST(ContextMenu, HideRaised)
{
    ContextMenu menu;
    menu.set_visible(true);
    menu.set_hide_enabled(true);

    TestImgui imgui([&]() { menu.render(); });

    bool raised = false;
    auto token = menu.on_hide += [&raised]() { raised = true; };

    imgui.show_context_menu("Debug##Default");
    imgui.click_element(imgui.popup_id("void_context").id(ContextMenu::Names::hide), true);
    imgui.render();

    ASSERT_TRUE(raised);
    ASSERT_FALSE(menu.visible());
}

TEST(ContextMenu, OrbitHereRaised)
{
    ContextMenu menu;
    menu.set_visible(true);

    TestImgui imgui([&]() { menu.render(); });

    bool raised = false;
    auto token = menu.on_orbit_here += [&raised]() { raised = true; };

    imgui.show_context_menu("Debug##Default");
    imgui.click_element(imgui.popup_id("void_context").id(ContextMenu::Names::orbit), true);
    imgui.render();

    ASSERT_TRUE(raised);
    ASSERT_FALSE(menu.visible());
}

TEST(ContextMenu, RemoveWaypointDisabled)
{
    ContextMenu menu;
    menu.set_visible(true);
    menu.set_remove_enabled(true);

    TestImgui imgui([&]() { menu.render(); });

    imgui.show_context_menu("Debug##Default");
    const auto original_flags = imgui.item_flags(imgui.popup_id("void_context").id(ContextMenu::Names::remove_waypoint));
    ASSERT_FALSE(original_flags & ImGuiItemFlags_Disabled);

    menu.set_remove_enabled(false);
    imgui.render();

    const auto new_flags = imgui.item_flags(imgui.popup_id("void_context").id(ContextMenu::Names::remove_waypoint));
    ASSERT_TRUE(new_flags & ImGuiItemFlags_Disabled);
}

TEST(ContextMenu, RemoveWaypointNotRaisedWhenDisabled)
{
    ContextMenu menu;
    menu.set_visible(true);
    menu.set_remove_enabled(false);

    TestImgui imgui([&]() { menu.render(); });

    bool raised = false;
    auto token = menu.on_remove_waypoint += [&raised]() { raised = true; };

    imgui.show_context_menu("Debug##Default");
    imgui.click_element(imgui.popup_id("void_context").id(ContextMenu::Names::remove_waypoint), true);

    ASSERT_FALSE(raised);
    ASSERT_TRUE(menu.visible());
}

TEST(ContextMenu, RemoveWaypointRaised)
{
    ContextMenu menu;
    menu.set_visible(true);
    menu.set_remove_enabled(true);

    TestImgui imgui([&]() { menu.render(); });

    bool raised = false;
    auto token = menu.on_remove_waypoint += [&raised]() { raised = true; };

    imgui.show_context_menu("Debug##Default");
    imgui.click_element(imgui.popup_id("void_context").id(ContextMenu::Names::remove_waypoint), true);
    imgui.render();

    ASSERT_TRUE(raised);
    ASSERT_FALSE(menu.visible());
}

TEST(ContextMenu, AddMidWaypointRaised)
{
    ContextMenu menu;
    menu.set_visible(true);
    menu.set_mid_waypoint_enabled(true);

    TestImgui imgui([&]() { menu.render(); });

    bool raised = false;
    auto token = menu.on_add_mid_waypoint += [&raised]() { raised = true; };

    imgui.show_context_menu("Debug##Default");
    imgui.click_element(imgui.popup_id("void_context").id(ContextMenu::Names::add_mid_waypoint), true);
    imgui.render();

    ASSERT_TRUE(raised);
    ASSERT_FALSE(menu.visible());
}

TEST(ContextMenu, AddMidWaypointDisabled)
{
    ContextMenu menu;
    menu.set_visible(true);
    menu.set_mid_waypoint_enabled(true);

    TestImgui imgui([&]() { menu.render(); });

    imgui.show_context_menu("Debug##Default");
    const auto original_flags = imgui.item_flags(imgui.popup_id("void_context").id(ContextMenu::Names::add_mid_waypoint));
    ASSERT_FALSE(original_flags & ImGuiItemFlags_Disabled);

    menu.set_mid_waypoint_enabled(false);
    imgui.render();

    const auto new_flags = imgui.item_flags(imgui.popup_id("void_context").id(ContextMenu::Names::add_mid_waypoint));
    ASSERT_TRUE(new_flags & ImGuiItemFlags_Disabled);
}

TEST(ContextMenu, AddMidWaypointNotRaisedWhenDisabled)
{
    ContextMenu menu;
    menu.set_visible(true);
    menu.set_mid_waypoint_enabled(false);

    TestImgui imgui([&]() { menu.render(); });

    bool raised = false;
    auto token = menu.on_add_mid_waypoint += [&raised]() { raised = true; };

    imgui.show_context_menu("Debug##Default");
    imgui.click_element(imgui.popup_id("void_context").id(ContextMenu::Names::add_mid_waypoint), true);

    ASSERT_FALSE(raised);
    ASSERT_TRUE(menu.visible());
}

TEST(ContextMenu, CopyPosition)
{
    FAIL();
}

TEST(ContextMenu, CopyRoom)
{
    FAIL();
}
