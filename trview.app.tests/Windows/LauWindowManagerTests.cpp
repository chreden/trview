#include <trview.app/Windows/LauWindowManager.h>
#include <trview.app/Mocks/Windows/ILauWindow.h>

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
            Window window{ create_test_window(L"LauWindowManagerTests") };
            std::shared_ptr<MockShortcuts> shortcuts{ std::make_shared<MockShortcuts>() };
            ILauWindow::Source window_source{ [](auto&&...) { return std::make_shared<MockLauWindow>(); } };

            test_module& with_window_source(const ILauWindow::Source& source)
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

            std::unique_ptr<LauWindowManager> build()
            {
                return std::make_unique<LauWindowManager>(window, shortcuts, window_source);
            }
        };

        return test_module{};
    }
}

TEST(LauWindowManager, CreateWindowCreatesNewWindow)
{
    auto mock_window = std::make_shared<MockLauWindow>();
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto created = manager->create_window().lock();
    ASSERT_EQ(created, mock_window);
}

TEST(LauWindowManager, WindowsRendered)
{
    auto mock_window = std::make_shared<MockLauWindow>();
    EXPECT_CALL(*mock_window, render(true)).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto created = manager->create_window().lock();
    ASSERT_EQ(created, mock_window);

    manager->render(true);
}
