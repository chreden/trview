#include <trview.app/Windows/RoomsWindowManager.h>
#include <trview.app/Elements/Types.h>
#include <trview.app/Mocks/Windows/IRoomsWindow.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Elements/ITrigger.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

TEST(RoomsWindowManager, SetTriggersClearsSelectedTrigger)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockRoomsWindow>();
    EXPECT_CALL(*mock_window, set_triggers).Times(3);
    EXPECT_CALL(*mock_window, clear_selected_trigger).Times(2);
    RoomsWindowManager manager(create_test_window(L"RoomsWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    auto trigger = std::make_shared<MockTrigger>();
    manager.set_triggers({ trigger });

    ASSERT_EQ(manager.selected_trigger().lock(), nullptr);
    manager.set_selected_trigger(trigger);
    ASSERT_EQ(manager.selected_trigger().lock(), trigger);
    manager.set_triggers({});
    ASSERT_EQ(manager.selected_trigger().lock(), nullptr);
}

TEST(RoomsWindowManager, WindowsUpdated)
{
    FAIL();
}
