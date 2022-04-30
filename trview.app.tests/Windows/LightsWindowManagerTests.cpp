#include <trview.app/Windows/LightsWindowManager.h>
#include <trview.app/Mocks/Windows/ILightsWindow.h>
#include <trview.app/Mocks/Elements/ILight.h>

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
            Window window{ create_test_window(L"ItemsWindowManagerTests") };
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            ILightsWindow::Source window_source{ [](auto&&...) { return mock_shared<MockLightsWindow>(); } };

            test_module& with_window_source(const ILightsWindow::Source& source)
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

            std::unique_ptr<LightsWindowManager> build()
            {
                return std::make_unique<LightsWindowManager>(window, shortcuts, window_source);
            }
        };

        return test_module{};
    }
}

TEST(LightsWindowManager, OnLightSelectedRaised)
{
    auto manager = register_test_module().build();
    auto light = mock_shared<MockLight>();

    std::weak_ptr<ILight> raised;
    auto token = manager->on_light_selected += [&](auto&& value) { raised = value; };

    auto window = manager->create_window().lock();
    ASSERT_NE(window, nullptr);

    window->on_light_selected(light);
    ASSERT_EQ(raised.lock(), light);
}

TEST(LightsWindowManager, OnLightVisibilityRaised)
{
    auto manager = register_test_module().build();
    auto light = mock_shared<MockLight>();

    std::optional<std::tuple<std::weak_ptr<ILight>, bool>> raised;
    auto token = manager->on_light_visibility += [&](auto&& value, auto&& visible) 
    {
        raised = { value, visible };
    };

    auto window = manager->create_window().lock();
    ASSERT_NE(window, nullptr);

    window->on_light_visibility(light, true);
    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(std::get<0>(raised.value()).lock(), light);
    ASSERT_TRUE(std::get<1>(raised.value()));
}

TEST(LightsWindowManager, SetLightsUpdatesExistingWindows)
{
    auto window = mock_shared<MockLightsWindow>();
    EXPECT_CALL(*window, set_lights).Times(2);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
    manager->set_lights({ mock_shared<MockLight>() });
}

TEST(LightsWindowManager, SetLightsUpdatesNewWindows)
{
    auto window = mock_shared<MockLightsWindow>();
    EXPECT_CALL(*window, set_lights).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
}

TEST(LightsWindowManager, RendersAllWindows)
{
    auto window = mock_shared<MockLightsWindow>();
    EXPECT_CALL(*window, render).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
    manager->render();
}

TEST(LightsWindowManager, UpdatesAllWindows)
{
    auto window = mock_shared<MockLightsWindow>();
    EXPECT_CALL(*window, update(1.0f)).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
    manager->update(1.0f);
}

TEST(LightsWindowManager, SetSelectedLightUpdatesExistingWindows)
{
    auto window = mock_shared<MockLightsWindow>();
    EXPECT_CALL(*window, set_selected_light).Times(2);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
    manager->set_selected_light(mock_shared<MockLight>());
}

TEST(LightsWindowManager, SetSelectedLightUpdatesNewWindows)
{
    auto window = mock_shared<MockLightsWindow>();
    EXPECT_CALL(*window, set_selected_light).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
}

TEST(LightsWindowManager, SetLevelVersionUpdatesExistingWindows)
{
    auto window = mock_shared<MockLightsWindow>();
    EXPECT_CALL(*window, set_level_version(trlevel::LevelVersion::Tomb1)).Times(1);
    EXPECT_CALL(*window, set_level_version(trlevel::LevelVersion::Tomb4)).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->set_level_version(trlevel::LevelVersion::Tomb1);
    manager->create_window();
    manager->set_level_version(trlevel::LevelVersion::Tomb4);
}

TEST(LightsWindowManager, SetLevelVersionUpdatesNewWindows)
{
    auto window = mock_shared<MockLightsWindow>();
    EXPECT_CALL(*window, set_level_version(trlevel::LevelVersion::Tomb4)).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->set_level_version(trlevel::LevelVersion::Tomb4);
    manager->create_window();
}

TEST(LightsWindowManager, SetRoomUpdatesExistingWindows)
{
    auto window = mock_shared<MockLightsWindow>();
    EXPECT_CALL(*window, set_current_room(0)).Times(1);
    EXPECT_CALL(*window, set_current_room(50)).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
    manager->set_room(50);
}

TEST(LightsWindowManager, SetRoomUpdatesNewWindows)
{
    auto window = mock_shared<MockLightsWindow>();
    EXPECT_CALL(*window, set_current_room(50)).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->set_room(50);
    manager->create_window();
}

TEST(LightsWindowManager, CreateWindowSetsNumber)
{
    auto window = mock_shared<MockLightsWindow>();
    EXPECT_CALL(*window, set_number(1)).Times(1);

    auto manager = register_test_module().with_window_source([&]() { return window; }).build();
    manager->create_window();
}

TEST(LightsWindowManager, CreateLightsWindowKeyboardShortcut)
{
    auto shortcuts = mock_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut(true, 'L')).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto manager = register_test_module().with_shortcuts(shortcuts).build();
}
