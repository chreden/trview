#include "gtest/gtest.h"
#include <trview.app/Menus/MenuDetector.h>
#include <trview.tests.common/Window.h>
#include <optional>

using namespace trview;
using namespace trview::tests;

TEST(MenuDetector, OpenMenu)
{
    MenuDetector detector(create_test_window(L"MenuDetector"));

    std::optional<bool> value;
    auto token = detector.on_menu_toggled += [&](bool open)
    {
        value = open;
    };

    detector.process_message(WM_ENTERMENULOOP, 0, 0);

    ASSERT_TRUE(value.has_value());
    ASSERT_TRUE(value.value());
}

TEST(MenuDetector, CloseMenu)
{
    MenuDetector detector(create_test_window(L"MenuDetector"));

    std::optional<bool> value;
    auto token = detector.on_menu_toggled += [&](bool open)
    {
        value = open;
    };

    detector.process_message(WM_EXITMENULOOP, 0, 0);

    ASSERT_TRUE(value.has_value());
    ASSERT_FALSE(value.value());
}