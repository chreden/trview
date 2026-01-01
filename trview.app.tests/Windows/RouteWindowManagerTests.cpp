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
            Window window{ create_test_window(L"RouteWindowManagerTests") };chrome://vivaldi-webui/startpage?section=Speed-dials&background-color=#2e2f37
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            RouteWindow::Source window_source{ [](auto&&...) { return mock_shared<MockRouteWindow>(); } };

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
    auto mock_window = mock_shared<MockRouteWindow>();
    EXPECT_CALL(*mock_window, update(1.0f)).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->update(1.0f);
}

TEST(RouteWindowManager, OnWindowCreatedEventRaised)
{
    auto manager = register_test_module().build();

    bool raised = false;
    auto token = manager->on_window_created += [&]() { raised = true; };

    manager->create_window();

    ASSERT_TRUE(raised);
}

TEST(RouteWindowManager, IsWindowOpen)
{
    auto mock_window = mock_shared<MockRouteWindow>();
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    ASSERT_FALSE(manager->is_window_open());
    manager->create_window();
    ASSERT_TRUE(manager->is_window_open());
    mock_window->on_window_closed();
    manager->render();
    ASSERT_FALSE(manager->is_window_open());
}

TEST(RouteWindowManager, OnRouteOpenRaised)
{
    auto mock_window = mock_shared<MockRouteWindow>();
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    bool raised = false;
    auto token = manager->on_route_open += [&]() { raised = true; };

    manager->create_window();
    mock_window->on_route_open();

    ASSERT_TRUE(raised);
}

TEST(RouteWindowManager, OnRouteReloadRaised)
{
    auto mock_window = mock_shared<MockRouteWindow>();
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    bool raised = false;
    auto token = manager->on_route_reload += [&]() { raised = true; };

    manager->create_window();
    mock_window->on_route_reload();

    ASSERT_TRUE(raised);
}

TEST(RouteWindowManager, OnRouteSaveRaised)
{
    auto mock_window = mock_shared<MockRouteWindow>();
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    bool raised = false;
    auto token = manager->on_route_save += [&]() { raised = true; };

    manager->create_window();
    mock_window->on_route_save();

    ASSERT_TRUE(raised);
}

TEST(RouteWindowManager, OnRouteSaveAsRaised)
{
    auto mock_window = mock_shared<MockRouteWindow>();
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    bool raised = false;
    auto token = manager->on_route_save_as += [&]() { raised = true; };

    manager->create_window();
    mock_window->on_route_save_as();

    ASSERT_TRUE(raised);
}

TEST(RouteWindowManager, OnLevelSwitchRaised)
{
    auto mock_window = mock_shared<MockRouteWindow>();
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    std::optional<std::string> filename;
    auto token = manager->on_level_switch += [&](const auto& f) { filename = f; };

    manager->create_window();
    mock_window->on_level_switch("test1");

    ASSERT_TRUE(filename);
    ASSERT_EQ(filename.value(), "test1");
}


TEST(RouteWindowManager, OnNewRouteRaised)
{
    auto mock_window = mock_shared<MockRouteWindow>();
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    bool raised = false;
    auto token = manager->on_new_route += [&]() { raised = true; };

    manager->create_window();
    mock_window->on_new_route();

    ASSERT_TRUE(raised);
}

TEST(RouteWindowManager, OnNewRandomizerRouteRaised)
{
    auto mock_window = mock_shared<MockRouteWindow>();
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    bool raised = false;
    auto token = manager->on_new_randomizer_route += [&]() { raised = true; };

    manager->create_window();
    mock_window->on_new_randomizer_route();

    ASSERT_TRUE(raised);
}
