#include <trview.app/Windows/Log/LogWindowManager.h>
#include <trview.app/Mocks/Windows/ILogWindow.h>
#include <trview.app/Resources/resource.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

namespace
{
    Event<> shortcut_handler;

    auto register_test_module()
    {
        struct test_module
        {
            Window window{ create_test_window(L"LogWindowManagerTests") };
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            ILogWindow::Source window_source{ [](auto&&...) { return mock_shared<MockLogWindow>(); } };

            test_module& with_window_source(const ILogWindow::Source& source)
            {
                this->window_source = source;
                return *this;
            }

            test_module& with_shortcuts(const std::shared_ptr<MockShortcuts>& shortcuts)
            {
                this->shortcuts = shortcuts;
                return *this;
            }

            test_module()
            {
                EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
            }

            std::unique_ptr<LogWindowManager> build()
            {
                return std::make_unique<LogWindowManager>(window, shortcuts, window_source);
            }
        };

        return test_module{};
    }
}

TEST(LogWindowManager, RendersAllWindows)
{
    auto window = mock_shared<MockLogWindow>();
    EXPECT_CALL(*window, render).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
    manager->render();
}

TEST(LogWindowManager, CreateWindowSetsNumber)
{
    auto window = mock_shared<MockLogWindow>();
    EXPECT_CALL(*window, set_number(1)).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
}

TEST(LogWindowManager, WindowCreatedOnCommand)
{
    auto window = mock_shared<MockLogWindow>();
    EXPECT_CALL(*window, set_number(1)).Times(1);
    EXPECT_CALL(*window, render).Times(1);
    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_LOG, 0), 0);
    manager->render();
}
