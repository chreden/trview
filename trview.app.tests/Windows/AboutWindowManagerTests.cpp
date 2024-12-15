#include <trview.app/Windows/About/AboutWindowManager.h>
#include <trview.app/Mocks/Windows/IAboutWindow.h>
#include <trview.app/Resources/resource.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            Window window{ create_test_window(L"AboutWindowManagerTests") };
            IAboutWindow::Source window_source{ [](auto&&...) { return mock_shared<MockAboutWindow>(); } };

            test_module& with_window_source(const IAboutWindow::Source& source)
            {
                this->window_source = source;
                return *this;
            }

            std::unique_ptr<AboutWindowManager> build()
            {
                return std::make_unique<AboutWindowManager>(window, window_source);
            }
        };

        return test_module{};
    }
}

TEST(AboutWindowManager, WindowCreatedOnCommand)
{
    auto window = mock_shared<MockAboutWindow>();
    bool raised = false;
    auto manager = register_test_module()
        .with_window_source([&]() { raised = true; return window; })
        .build();

    manager->process_message(WM_COMMAND, MAKEWPARAM(IDM_ABOUT, 0), 0);

    ASSERT_TRUE(raised);
}

TEST(AboutWindowManager, ExistingWindowFocused)
{
    auto window = mock_shared<MockAboutWindow>();
    EXPECT_CALL(*window, focus).Times(1);

    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

    manager->create_window();
    manager->create_window();
}

TEST(AboutWindowManager, WindowRendered)
{
    auto window = mock_shared<MockAboutWindow>();
    EXPECT_CALL(*window, render).Times(1);

    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

    manager->create_window();
    manager->render();
}

TEST(AboutWindowManager, ClosedForwarded)
{
    auto window = mock_shared<MockAboutWindow>();

    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

    manager->create_window();

    std::weak_ptr<IAboutWindow> window_weak = window;
    window->on_window_closed();
    window.reset();
    manager->render();
    ASSERT_EQ(window_weak.lock(), nullptr);
}