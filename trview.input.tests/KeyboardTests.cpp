#include "gtest/gtest.h"
#include <trview.input/Keyboard.h>
#include <trview.tests.common/Window.h>

using namespace trview::tests;
using namespace trview::input;

/// Tests that when a WM_KEYDOWN message is posted to the window that the keyboard is watching
/// the key down event is raised.
TEST(Keyboard, KeyDownEventRaised)
{
    int times_called = 0;
    uint16_t key_received = 0;

    Keyboard keyboard(create_test_window(L"TRViewInputTests"));
    auto token = keyboard.on_key_down +=
        [&times_called, &key_received](uint16_t key, bool control)
    {
        ++times_called;
        key_received = key;
    };

    keyboard.process_message(WM_KEYDOWN, VK_SPACE, 0);

    ASSERT_EQ(1, times_called);
    ASSERT_EQ(static_cast<int>(VK_SPACE), static_cast<int>(key_received));
}

/// Tests that when a WM_KEYUP message is posted to the window that the keyboard is watching
/// the key up event is raised.
TEST(Keyboard, KeyUpEventRaised)
{
    int times_called = 0;
    uint16_t key_received = 0;

    Keyboard keyboard(create_test_window(L"TRViewInputTests"));
    auto token = keyboard.on_key_up +=
        [&times_called, &key_received](uint16_t key, bool control)
    {
        ++times_called;
        key_received = key;
    };

    keyboard.process_message(WM_KEYUP, VK_SPACE, 0);

    ASSERT_EQ(1, times_called);
    ASSERT_EQ(static_cast<int>(VK_SPACE), static_cast<int>(key_received));
}

/// Tests that when a WM_CHAR message is posted to the window that the keyboard is watching
/// the char event is raised.
TEST(Keyboard, OnCharEventRaised)
{
    int times_called = 0;
    uint16_t char_received = 0;

    Keyboard keyboard(create_test_window(L"TRViewInputTests"));
    auto token = keyboard.on_char +=
        [&times_called, &char_received](uint16_t key)
    {
        ++times_called;
        char_received = key;
    };

    keyboard.process_message(WM_CHAR, 'A', 0);

    ASSERT_EQ(1, times_called);
    ASSERT_EQ(static_cast<int>('A'), static_cast<int>(char_received));
}
