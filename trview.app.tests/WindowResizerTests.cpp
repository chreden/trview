#include <trview.app/Windows/WindowResizer.h>

using namespace trview;
using namespace trview::tests;

// Tests that entering and exiting resizing raises the resize event.
TEST(WindowResizer, SizeMove)
{
    WindowResizer resizer(create_test_window(L"TRViewWindowResizerTests"));

    uint32_t times_called = 0;
    auto token = resizer.on_resize += [&]() { ++times_called; };

    resizer.process_message(WM_ENTERSIZEMOVE, 0, 0);
    resizer.process_message(WM_EXITSIZEMOVE, 0, 0);

    ASSERT_EQ(1u, times_called);
}

// Test that sending a maximize message triggers the event.
TEST(WindowResizer, Maximise)
{
    WindowResizer resizer(create_test_window(L"TRViewWindowResizerTests"));

    uint32_t times_called = 0;
    auto token = resizer.on_resize += [&]() { ++times_called; };

    resizer.process_message(WM_SIZE, SIZE_MAXIMIZED, 0);

    ASSERT_EQ(1u, times_called);
}

// Test that sending a minimise message triggers the event.
TEST(WindowResizer, Minimise)
{
    WindowResizer resizer(create_test_window(L"TRViewWindowResizerTests"));

    uint32_t times_called = 0;
    auto token = resizer.on_resize += [&]() { ++times_called; };

    resizer.process_message(WM_SIZE, SIZE_RESTORED, 0);

    ASSERT_EQ(1u, times_called);
}

// Test that sending a minimise message notifies owned windows.
TEST(WindowResizer, MinimiseNotifiesOwnedWindows)
{
    auto parent = create_test_window(L"TRViewWindowResizerTests");
    WindowResizer resizer(parent);

    uint32_t times_called = 0;
    auto owned_window = create_test_window(L"TRViewWindowResizerTests-Child", parent);
    SetWindowSubclass(owned_window, 
        [](auto, auto message, auto, auto, auto, auto ref_data) -> LRESULT 
        {
            if (message == WindowResizer::WM_APP_PARENT_RESTORED)
            {
                ++(*reinterpret_cast<uint32_t*>(ref_data));
            }
            return 0;
        }, 1337, reinterpret_cast<DWORD_PTR>(&times_called));

    resizer.process_message(WM_SIZE, SIZE_RESTORED, 0);
    ASSERT_EQ(1u, times_called);
}

// Test that sending a app restore message triggers the resize event.
TEST(WindowResizer, AppRestoreTriggersOnResize)
{
    WindowResizer resizer(create_test_window(L"TRViewWindowResizerTests"));

    uint32_t times_called = 0;
    auto token = resizer.on_resize += [&]() { ++times_called; };

    resizer.process_message(WindowResizer::WM_APP_PARENT_RESTORED, 0, 0);

    ASSERT_EQ(1u, times_called);
}

// Test that size messages during resize don't trigger the event.
TEST(WindowResizer, Resizing)
{
    WindowResizer resizer(create_test_window(L"TRViewWindowResizerTests"));

    uint32_t times_called = 0;
    auto token = resizer.on_resize += [&]() { ++times_called; };

    resizer.process_message(WM_ENTERSIZEMOVE, 0, 0);
    ASSERT_EQ(0u, times_called);

    resizer.process_message(WM_SIZE, 0, 0);
    ASSERT_EQ(0u, times_called);

    resizer.process_message(WM_EXITSIZEMOVE, 0, 0);
    ASSERT_EQ(1u, times_called);
}