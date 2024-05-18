#include <trview.app/Windows/Statics/StaticsWindowManager.h>
#include <trview.app/Mocks/Windows/IStaticsWindow.h>
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
            Window window{ create_test_window(L"StaticsWindowManagerTests") };
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            IStaticsWindow::Source window_source{ [](auto&&...) { return mock_shared<MockStaticsWindow>(); } };

            test_module& with_window_source(const IStaticsWindow::Source& source)
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

            std::unique_ptr<StaticsWindowManager> build()
            {
                return std::make_unique<StaticsWindowManager>(window, shortcuts, window_source);
            }
        };

        return test_module{};
    }
}

TEST(StaticsWindowManager, WindowCreatedOnCommand)
{
    auto window = mock_shared<MockStaticsWindow>();
    bool raised = false;
    auto manager = register_test_module()
        .with_window_source([&]() { raised = true; return window; })
        .build();

    manager->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_STATICS, 0), 0);

    ASSERT_TRUE(raised);
}

TEST(StaticsWindowManager, WindowCreated)
{
    auto window = mock_shared<MockStaticsWindow>();
    EXPECT_CALL(*window, set_number(1)).Times(1);

    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

    auto created = manager->create_window().lock();
    ASSERT_EQ(created, window);
}

TEST(StaticsWindowManager, RendersAllWindows)
{
    auto window = mock_shared<MockStaticsWindow>();
    EXPECT_CALL(*window, render).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
    manager->render();
}

TEST(StaticsWindowManager, CreateStaticsWindowKeyboardShortcut)
{
    auto shortcuts = mock_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut(true, 'S')).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto manager = register_test_module().with_shortcuts(shortcuts).build();
}
