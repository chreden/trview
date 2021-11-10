#include <trview.app/Windows/RouteWindowManager.h>
#include <trview.app/Elements/Types.h>
#include <trview.app/Mocks/Windows/IRouteWindow.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Elements/ITrigger.h>

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
            Window window{ create_test_window(L"RouteWindowManagerTests") };
            std::shared_ptr<MockShortcuts> shortcuts{ std::make_shared<MockShortcuts>() };
            RouteWindow::Source window_source{ [](auto&&...) { return std::make_shared<MockRouteWindow>(); } };

            test_module& with_window_source(const RouteWindow::Source& source)
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

            std::unique_ptr<RouteWindowManager> build()
            {
                return std::make_unique<RouteWindowManager>(window, shortcuts, window_source);
            }
        };

        return test_module{};
    }
}

TEST(RouteWindowManager, WindowsUpdated)
{
    auto mock_window = std::make_shared<MockRouteWindow>();
    EXPECT_CALL(*mock_window, update(1.0f)).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->update(1.0f);
}

TEST(RouteWindowManager, RandomizerEnabledPassed)
{
    FAIL();
}

TEST(RouteWindowManager, RandomizerEnabledPassedToNewWindows)
{
    FAIL();
}

TEST(RouteWindowManager, RandomizerSettingsPassed)
{
    FAIL();
}

TEST(RouteWindowManager, RandomizerSettingsPassedToNewWindow)
{
    FAIL();
}
