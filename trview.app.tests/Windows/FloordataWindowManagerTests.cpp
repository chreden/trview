#include <trview.app/Windows/Floordata/FloordataWindowManager.h>
#include <trview.app/Mocks/Windows/IFloordataWindow.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

namespace
{
    Event<> shortcut_handler;

    auto register_test_module()
    {
        struct test_module
        {
            Window window{ create_test_window(L"FloordataWindowManagerTests") };
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            IFloordataWindow::Source window_source{ [](auto&&...) { return mock_shared<MockFloordataWindow>(); } };

            test_module& with_window_source(const IFloordataWindow::Source& source)
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

            std::unique_ptr<FloordataWindowManager> build()
            {
                return std::make_unique<FloordataWindowManager>(window, shortcuts, window_source);
            }
        };

        return test_module{};
    }
}

TEST(FloordataWindowManager, WindowsUpdated)
{
    FAIL();
}

TEST(FloordataWindowManager, FloordataPassedToNewWindows)
{
    FAIL();
}
