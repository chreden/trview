#include <trview.app/Windows/RoomsWindowManager.h>
#include <trview.app/Elements/Types.h>
#include <trview.app/Mocks/Windows/IRoomsWindow.h>
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
            Window window{ create_test_window(L"RoomsWindowManagerTests") };
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            RoomsWindow::Source window_source{ [](auto&&...) { return mock_shared<MockRoomsWindow>(); } };

            test_module& with_window_source(const RoomsWindow::Source& source)
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

            std::unique_ptr<RoomsWindowManager> build()
            {
                return std::make_unique<RoomsWindowManager>(window, shortcuts, window_source);
            }
        };

        return test_module{};
    }
}

TEST(RoomsWindowManager, SetTriggersClearsSelectedTrigger)
{
    auto mock_window = mock_shared<MockRoomsWindow>();
    EXPECT_CALL(*mock_window, set_triggers).Times(3);
    EXPECT_CALL(*mock_window, clear_selected_trigger).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    auto trigger = mock_shared<MockTrigger>();
    manager->set_triggers({ trigger });

    ASSERT_EQ(manager->selected_trigger().lock(), nullptr);
    manager->set_selected_trigger(trigger);
    ASSERT_EQ(manager->selected_trigger().lock(), trigger);
    manager->set_triggers({});
    ASSERT_EQ(manager->selected_trigger().lock(), nullptr);
}

TEST(RoomsWindowManager, WindowsUpdated)
{
    auto mock_window = mock_shared<MockRoomsWindow>();
    EXPECT_CALL(*mock_window, update(1.0f)).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->update(1.0f);
}

TEST(RoomsWindowManager, LevelVersionPassedToNewWindows)
{
    auto mock_window = mock_shared<MockRoomsWindow>();
    EXPECT_CALL(*mock_window, set_level_version(trlevel::LevelVersion::Tomb3)).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->set_level_version(trlevel::LevelVersion::Tomb3);
    manager->create_window();
}

TEST(RoomsWindowManager, LevelVersionPassedToWindows)
{
    auto mock_window = mock_shared<MockRoomsWindow>();
    EXPECT_CALL(*mock_window, set_level_version(trlevel::LevelVersion::Unknown)).Times(1);
    EXPECT_CALL(*mock_window, set_level_version(trlevel::LevelVersion::Tomb3)).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->set_level_version(trlevel::LevelVersion::Tomb3);
}

TEST(RoomsWindowManager, SetMapColoursPassedToWindows)
{
    auto mock_window = mock_shared<MockRoomsWindow>();
    EXPECT_CALL(*mock_window, set_map_colours).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->set_map_colours({});
}

TEST(RoomsWindowManager, MapColoursPassedToNewWindows)
{
    auto mock_window = mock_shared<MockRoomsWindow>();
    EXPECT_CALL(*mock_window, set_map_colours).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
}

TEST(RoomsWindowManager, OnRoomVisibilityRaised)
{
    auto manager = register_test_module().build();
    auto room = mock_shared<MockRoom>();

    std::optional<std::tuple<std::weak_ptr<IRoom>, bool>> raised;
    auto token = manager->on_room_visibility += [&](auto&& value, auto&& visible)
    {
        raised = { value, visible };
    };

    auto window = manager->create_window().lock();
    ASSERT_NE(window, nullptr);

    window->on_room_visibility(room, true);
    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(std::get<0>(raised.value()).lock(), room);
    ASSERT_TRUE(std::get<1>(raised.value()));
}
