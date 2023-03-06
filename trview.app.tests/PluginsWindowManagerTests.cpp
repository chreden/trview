#include <trview.app/Windows/Plugins/PluginsWindowManager.h>
#include <trview.app/Mocks/Windows/IPluginsWindow.h>
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
            Window window{ create_test_window(L"CameraSinkWindowManagerTests") };
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            IPluginsWindow::Source window_source{ [](auto&&...) { return mock_shared<MockPluginsWindow>(); } };

            test_module& with_window_source(const IPluginsWindow::Source& source)
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

            std::unique_ptr<PluginsWindowManager> build()
            {
                return std::make_unique<PluginsWindowManager>(window, shortcuts, window_source);
            }
        };

        return test_module{};
    }
}

TEST(PluginsWindowManager, WindowCreatedOnCommand)
{
    auto window = mock_shared<MockPluginsWindow>();
    bool raised = false;
    auto manager = register_test_module()
        .with_window_source([&]() { raised = true; return window; })
        .build();

    manager->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_PLUGINS, 0), 0);

    ASSERT_TRUE(raised);
}

TEST(PluginsWindowManager, WindowCreated)
{
    auto window = mock_shared<MockPluginsWindow>();
    EXPECT_CALL(*window, set_number(1)).Times(1);

    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

    auto created = manager->create_window().lock();
    ASSERT_EQ(created, window);
}

TEST(PluginsWindowManager, RendersAllWindows)
{
    auto window = mock_shared<MockPluginsWindow>();
    EXPECT_CALL(*window, render).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
    manager->render();
}

TEST(PluginsWindowManager, CreateLightsWindowKeyboardShortcut)
{
    auto shortcuts = mock_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut(true, 'P')).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto manager = register_test_module().with_shortcuts(shortcuts).build();
}
