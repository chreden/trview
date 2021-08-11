#include <trview.input/WindowTester.h>
#include <trview.tests.common/Window.h>

using namespace trview::tests;
using namespace trview::input;

/// Tests that the tester notices when the mouse enters the area.
TEST(WindowTester, MouseMoveDetected)
{
    auto tester = WindowTester(create_test_window(L"WindowTesterTests"));

    ASSERT_FALSE(tester.is_window_under_cursor());
    tester.process_message(WM_MOUSEMOVE, 0, 0);
    ASSERT_TRUE(tester.is_window_under_cursor());
}

/// Tests that the tester notices when the mouse leaves the area.
TEST(WindowTester, MouseExitDetected)
{
    auto tester = WindowTester(create_test_window(L"WindowTesterTests"));

    ASSERT_FALSE(tester.is_window_under_cursor());
    tester.process_message(WM_MOUSEMOVE, 0, 0);
    ASSERT_TRUE(tester.is_window_under_cursor());
    tester.process_message(WM_MOUSELEAVE, 0, 0);
    ASSERT_FALSE(tester.is_window_under_cursor());
}