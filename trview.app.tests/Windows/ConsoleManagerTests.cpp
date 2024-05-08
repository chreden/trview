#include <trview.app/Windows/Console/ConsoleManager.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.app/Mocks/Windows/IConsole.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using namespace testing;

namespace
{
    Event<> shortcut_handler;

    auto register_test_module()
    {
        struct test_module
        {
            Window window{ create_test_window(L"ConsoleManagerTests") };
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            IConsole::Source window_source{ [](auto&&...) { return mock_shared<MockConsole>(); } };
            std::shared_ptr<IFiles> files;

            test_module& with_window_source(const IConsole::Source& source)
            {
                this->window_source = source;
                return *this;
            }

            test_module& with_shortcuts(const std::shared_ptr<MockShortcuts>& shortcuts)
            {
                this->shortcuts = shortcuts;
                return *this;
            }

            test_module& with_files(const std::shared_ptr<IFiles>& files)
            {
                this->files = files;
                return *this;
            }

            test_module()
            {
                EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
            }

            std::unique_ptr<ConsoleManager> build()
            {
                return std::make_unique<ConsoleManager>(window, shortcuts, window_source, files);
            }
        };

        return test_module{};
    }
}

TEST(ConsoleManager, CreateConsoleKeyboardShortcut)
{
    auto shortcuts = mock_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut(false, VK_F11)).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto manager = register_test_module().with_shortcuts(shortcuts).build();
}

TEST(ConsoleManager, CreatedWindow)
{
    auto window = mock_shared<MockConsole>();
    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    auto result = manager->create_window();
    ASSERT_EQ(result.lock(), window);
}

TEST(ConsoleManager, CreateWindowSetsNumber)
{
    auto window = mock_shared<MockConsole>();
    EXPECT_CALL(*window, set_number(1)).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
}