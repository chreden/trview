#include <trview.app/Menus/ViewMenu.h>
#include <trview.app/Resources/resource.h>

using namespace trview;
using namespace trview::tests;

namespace
{
    Window create_menu_window(int id)
    {
        auto window = create_test_window(L"ViewMenuTests");
        auto win_menu = CreateMenu();
        AppendMenu(win_menu, MF_STRING, id, L"Text");
        SetMenu(window, win_menu);
        return window;
    }
}

TEST(ViewMenu, ShowMinimapRaised)
{
    ViewMenu menu(create_menu_window(ID_VIEW_MINIMAP));

    std::optional<bool> raised_value;
    auto token = menu.on_show_minimap += [&raised_value](bool value) { raised_value = value; };

    menu.process_message(WM_COMMAND, MAKEWPARAM(ID_VIEW_MINIMAP, 0), 0);
    ASSERT_TRUE(raised_value.has_value());
    ASSERT_FALSE(raised_value.value());

    menu.process_message(WM_COMMAND, MAKEWPARAM(ID_VIEW_MINIMAP, 0), 0);
    ASSERT_TRUE(raised_value.has_value());
    ASSERT_TRUE(raised_value.value());
}

TEST(ViewMenu, ShowTooltipRaised)
{
    ViewMenu menu(create_menu_window(ID_VIEW_TOOLTIP));

    std::optional<bool> raised_value;
    auto token = menu.on_show_tooltip += [&raised_value](bool value) { raised_value = value; };

    menu.process_message(WM_COMMAND, MAKEWPARAM(ID_VIEW_TOOLTIP, 0), 0);
    ASSERT_TRUE(raised_value.has_value());
    ASSERT_FALSE(raised_value.value());

    menu.process_message(WM_COMMAND, MAKEWPARAM(ID_VIEW_TOOLTIP, 0), 0);
    ASSERT_TRUE(raised_value.has_value());
    ASSERT_TRUE(raised_value.value());
}

TEST(ViewMenu, ShowUiRaised)
{
    ViewMenu menu(create_menu_window(ID_VIEW_UI));

    std::optional<bool> raised_value;
    auto token = menu.on_show_ui += [&raised_value](bool value) { raised_value = value; };

    menu.process_message(WM_COMMAND, MAKEWPARAM(ID_VIEW_UI, 0), 0);
    ASSERT_TRUE(raised_value.has_value());
    ASSERT_FALSE(raised_value.value());

    menu.process_message(WM_COMMAND, MAKEWPARAM(ID_VIEW_UI, 0), 0);
    ASSERT_TRUE(raised_value.has_value());
    ASSERT_TRUE(raised_value.value());
}

TEST(ViewMenu, ShowSelectionRaised)
{
    ViewMenu menu(create_menu_window(ID_VIEW_SELECTION));

    std::optional<bool> raised_value;
    auto token = menu.on_show_selection += [&raised_value](bool value) { raised_value = value; };

    menu.process_message(WM_COMMAND, MAKEWPARAM(ID_VIEW_SELECTION, 0), 0);
    ASSERT_TRUE(raised_value.has_value());
    ASSERT_FALSE(raised_value.value());

    menu.process_message(WM_COMMAND, MAKEWPARAM(ID_VIEW_SELECTION, 0), 0);
    ASSERT_TRUE(raised_value.has_value());
    ASSERT_TRUE(raised_value.value());
}

TEST(ViewMenu, ShowRouteRaised)
{
    ViewMenu menu(create_menu_window(ID_VIEW_ROUTE));

    std::optional<bool> raised_value;
    auto token = menu.on_show_route += [&raised_value](bool value) { raised_value = value; };

    menu.process_message(WM_COMMAND, MAKEWPARAM(ID_VIEW_ROUTE, 0), 0);
    ASSERT_TRUE(raised_value.has_value());
    ASSERT_FALSE(raised_value.value());

    menu.process_message(WM_COMMAND, MAKEWPARAM(ID_VIEW_ROUTE, 0), 0);
    ASSERT_TRUE(raised_value.has_value());
    ASSERT_TRUE(raised_value.value());
}

TEST(ViewMenu, ShowToolsRaised)
{
    ViewMenu menu(create_menu_window(ID_VIEW_TOOLS));

    std::optional<bool> raised_value;
    auto token = menu.on_show_tools += [&raised_value](bool value) { raised_value = value; };

    menu.process_message(WM_COMMAND, MAKEWPARAM(ID_VIEW_TOOLS, 0), 0);
    ASSERT_TRUE(raised_value.has_value());
    ASSERT_FALSE(raised_value.value());

    menu.process_message(WM_COMMAND, MAKEWPARAM(ID_VIEW_TOOLS, 0), 0);
    ASSERT_TRUE(raised_value.has_value());
    ASSERT_TRUE(raised_value.value());
}

TEST(ViewMenu, UnhideAllRaised)
{
    ViewMenu menu(create_menu_window(ID_VIEW_UNHIDE_ALL));

    bool raised;
    auto token = menu.on_unhide_all += [&raised]() { raised = true; };

    menu.process_message(WM_COMMAND, MAKEWPARAM(ID_VIEW_UNHIDE_ALL, 0), 0);
    ASSERT_TRUE(raised);
}
