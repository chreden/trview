#include "gtest/gtest.h"
#include <trview.input/Mouse.h>
#include <trview.tests.common/Window.h>

using namespace trview::tests;
using namespace trview::input;
using namespace trview;

namespace
{
    std::unique_ptr<IWindowTester> tester(const Window& window)
    {
        class TestWindowTester final : public IWindowTester
        {
        public:
            TestWindowTester(const Window& window)
                : _window(window)
            {
            }

            virtual Window window_under_cursor() const override
            {
                return _window;
            }

            virtual int screen_width(bool) const override
            {
                return 1000;
            }

            virtual int screen_height(bool) const override
            {
                return 1000;
            }
        private:
            Window _window;
        };
        return std::make_unique<TestWindowTester>(window);
    }
}

/// Tests that the mouse down event is raised when the mouse button message is
/// sent to the target window.
TEST(Mouse, MouseDownEventRaised)
{
    int times_called = 0;
    Mouse::Button button_received = Mouse::Button::Left;

    auto window = create_test_window(L"TRViewInputTests");
    Mouse mouse(window, tester(window));

    auto token = mouse.mouse_down +=
        [&times_called, &button_received](auto button)
    {
        ++times_called;
        button_received = button;
    };

    RAWINPUT input;
    memset(&input, 0, sizeof(input));
    input.header.dwType = RIM_TYPEMOUSE;
    input.header.dwSize = sizeof(RAWINPUT);
    input.data.mouse.usButtonFlags |= RI_MOUSE_RIGHT_BUTTON_DOWN;
    mouse.process_input(input);

    ASSERT_EQ(1, times_called);
    ASSERT_EQ(Mouse::Button::Right, button_received);
}

/// Tests that the mouse up event is raised when the mouse up message is 
/// passed to the window.
TEST(Mouse, MouseUpEventRaised)
{
    int times_called = 0;
    Mouse::Button button_received = Mouse::Button::Left;

    auto window = create_test_window(L"TRViewInputTests");
    Mouse mouse(window, tester(window));

    auto token = mouse.mouse_up +=
        [&times_called, &button_received](auto button)
    {
        ++times_called;
        button_received = button;
    };

    RAWINPUT input;
    memset(&input, 0, sizeof(input));
    input.header.dwType = RIM_TYPEMOUSE;
    input.header.dwSize = sizeof(RAWINPUT);
    input.data.mouse.usButtonFlags |= RI_MOUSE_RIGHT_BUTTON_UP;
    mouse.process_input(input);

    ASSERT_EQ(1, times_called);
    ASSERT_EQ(Mouse::Button::Right, button_received);
}

/// Tests that the mouse scroll event is raised when the mouse wheel event
/// is sent to the window.
TEST(Mouse, MouseWheelEventRaised)
{
    int times_called = 0;
    int16_t scroll_received = 0;

    auto window = create_test_window(L"TRViewInputTests");
    Mouse mouse(window, tester(window));

    auto token = mouse.mouse_wheel +=
        [&times_called, &scroll_received](auto scroll)
    {
        ++times_called;
        scroll_received = scroll;
    };

    mouse.process_message(WM_MOUSEWHEEL, MAKEWPARAM(0, 100), 0);

    ASSERT_EQ(1, times_called);
    ASSERT_EQ(static_cast<int>(100), static_cast<int>(scroll_received));
}

/// Tests that the mouse move event is raised when the mouse move message is
/// sent to the window.
TEST(Mouse, MouseMoveEventRaised)
{
    int times_called = 0;
    long x_received, y_received = 0;

    auto window = create_test_window(L"TRViewInputTests");
    Mouse mouse(window, tester(window));

    auto token = mouse.mouse_move +=
        [&times_called, &x_received, &y_received](long x, long y)
    {
        ++times_called;
        x_received = x;
        y_received = y;
    };

    RAWINPUT input;
    memset(&input, 0, sizeof(input));
    input.header.dwType = RIM_TYPEMOUSE;
    input.header.dwSize = sizeof(RAWINPUT);
    // Initial move - set the absolute position
    input.data.mouse.usFlags |= MOUSE_MOVE_ABSOLUTE;
    input.data.mouse.lLastX = 123;
    input.data.mouse.lLastY = 456;
    mouse.process_input(input);
    // Move relatively.
    input.data.mouse.usFlags = MOUSE_MOVE_RELATIVE;
    mouse.process_input(input);

    ASSERT_EQ(1, times_called);
    ASSERT_EQ(123, static_cast<int>(x_received));
    ASSERT_EQ(456, static_cast<int>(y_received));
}

/// Tests that the mouse reports the correct position after the mouse move message
/// is sent to the window.
TEST(Mouse, MousePositionCorrect)
{
    auto window = create_test_window(L"TRViewInputTests");
    Mouse mouse(window, tester(window));

    RAWINPUT input;
    memset(&input, 0, sizeof(input));
    input.header.dwType = RIM_TYPEMOUSE;
    input.header.dwSize = sizeof(RAWINPUT);
    input.data.mouse.usFlags |= MOUSE_MOVE_ABSOLUTE;
    input.data.mouse.lLastX = static_cast<int>(std::round((123.0f / 1000.0f) * 65535.0f));
    input.data.mouse.lLastY = static_cast<int>(std::round((456.0f / 1000.0f) * 65535.0f));
    mouse.process_input(input);

    ASSERT_EQ(123, static_cast<int>(mouse.x()));
    ASSERT_EQ(456, static_cast<int>(mouse.y()));
}
