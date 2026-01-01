#include <trview.app/Windows/Windows.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.app/Mocks/Windows/IWindow.h>
#include <trview.app/Resources/resource.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using namespace DirectX::SimpleMath;
using testing::A;
using testing::Return;

namespace
{
    Event<> shortcut_handler;

    auto register_test_module()
    {
        struct test_module
        {
            Window window{ create_test_window(L"WindowsTests") };
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            bool need_default{ true };

            std::unique_ptr<trview::Windows> build()
            {
                if (need_default)
                {
                    EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
                }
                return std::make_unique<trview::Windows>(window, shortcuts);
            }

            test_module& with_shortcuts(const std::shared_ptr<MockShortcuts>& shortcuts)
            {
                this->shortcuts = shortcuts;
                need_default = false;
                return *this;
            }
        };
        return test_module{};
    }
}

TEST(Windows, CameraSinkStartup)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("CameraSink", creator);
    windows->setup({ .camera_sink_startup = true });

    ASSERT_EQ(called, true);
}

TEST(Windows, ItemsStartup)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Items", creator);
    windows->setup({ .items_startup = true });

    ASSERT_EQ(called, true);
}

TEST(Windows, RoomsStartup)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Rooms", creator);
    windows->setup({ .rooms_startup = true });

    ASSERT_EQ(called, true);
}

TEST(Windows, RouteStartup)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Route", creator);
    windows->setup({ .route_startup = true });

    ASSERT_EQ(called, true);
}

TEST(Windows, StaticsStartup)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Statics", creator);
    windows->setup({ .statics_startup = true });

    ASSERT_EQ(called, true);
}

TEST(Windows, TriggersStartup)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Triggers", creator);
    windows->setup({ .triggers_startup = true });

    ASSERT_EQ(called, true);
}

TEST(Windows, AboutCreatedOnCommand)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("About", creator);
    windows->process_message(WM_COMMAND, MAKEWPARAM(IDM_ABOUT, 0), 0);
    ASSERT_EQ(called, true);
}

TEST(Windows, CameraSinkCreatedOnCommand)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("CameraSink", creator);
    windows->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_CAMERA_SINK, 0), 0);
    ASSERT_EQ(called, true);
}

TEST(Windows, ConsoleCreatedOnCommand)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Console", creator);
    windows->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_CONSOLE, 0), 0);
    ASSERT_EQ(called, true);
}

TEST(Windows, DiffCreatedOnCommand)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Diff", creator);
    windows->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_DIFF, 0), 0);
    ASSERT_EQ(called, true);
}

TEST(Windows, ItemsCreatedOnCommand)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Items", creator);
    windows->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_ITEMS, 0), 0);
    ASSERT_EQ(called, true);
}

TEST(Windows, LightsCreatedOnCommand)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Lights", creator);
    windows->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_LIGHTS, 0), 0);
    ASSERT_EQ(called, true);
}

TEST(Windows, LogCreatedOnCommand)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Log", creator);
    windows->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_LOG, 0), 0);
    ASSERT_EQ(called, true);
}

TEST(Windows, PackCreatedOnCommand)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Pack", creator);
    windows->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_PACK, 0), 0);
    ASSERT_EQ(called, true);
}

TEST(Windows, PluginsCreatedOnCommand)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Plugins", creator);
    windows->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_PLUGINS, 0), 0);
    ASSERT_EQ(called, true);
}

TEST(Windows, RoomsCreatedOnCommand)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Rooms", creator);
    windows->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_ROOMS, 0), 0);
    ASSERT_EQ(called, true);
}

TEST(Windows, RouteCreatedOnCommand)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Route", creator);
    windows->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_ROUTE, 0), 0);
    ASSERT_EQ(called, true);
}

TEST(Windows, SoundsCreatedOnCommand)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Sounds", creator);
    windows->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_SOUNDS, 0), 0);
    ASSERT_EQ(called, true);
}

TEST(Windows, StaticsCreatedOnCommand)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Statics", creator);
    windows->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_STATICS, 0), 0);
    ASSERT_EQ(called, true);
}

TEST(Windows, TexturesCreatedOnCommand)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Textures", creator);
    windows->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_TEXTURES, 0), 0);
    ASSERT_EQ(called, true);
}

TEST(Windows, TriggersCreatedOnCommand)
{
    auto window = mock_shared<MockWindow>();
    auto windows = register_test_module().build();

    bool called = false;
    auto creator = [&]()
        {
            called = true;
            return window;
        };

    windows->register_window("Triggers", creator);
    windows->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_TRIGGERS, 0), 0);
    ASSERT_EQ(called, true);
}

TEST(Windows, WindowsRendered)
{
    auto window = mock_shared<MockWindow>();
    EXPECT_CALL(*window, render).Times(1);

    auto window2 = mock_shared<MockWindow>();
    EXPECT_CALL(*window2, render).Times(1);

    auto windows = register_test_module().build();

    int32_t called_count = 0;
    auto creator = [&]()
        {
            ++called_count;
            return called_count == 1 ? window : window2;
        };

    windows->register_window("Triggers", creator);
    windows->create("Triggers");
    windows->create("Triggers");
    windows->render();

    ASSERT_EQ(called_count, 2);
}

TEST(Windows, WindowsUpdated)
{
    auto window = mock_shared<MockWindow>();
    EXPECT_CALL(*window, update).Times(1);

    auto window2 = mock_shared<MockWindow>();
    EXPECT_CALL(*window2, update).Times(1);

    auto windows = register_test_module().build();

    int32_t called_count = 0;
    auto creator = [&]()
        {
            ++called_count;
            return called_count == 1 ? window : window2;
        };

    windows->register_window("Triggers", creator);
    windows->create("Triggers");
    windows->create("Triggers");
    windows->update(1.0f);

    ASSERT_EQ(called_count, 2);
}

TEST(Windows, WindowNumbersAssigned)
{
    auto window = mock_shared<MockWindow>();
    EXPECT_CALL(*window, set_number(1)).Times(1);

    auto window2 = mock_shared<MockWindow>();
    EXPECT_CALL(*window2, set_number(2)).Times(1);

    auto windows = register_test_module().build();

    int32_t called_count = 0;
    auto creator = [&]()
        {
            ++called_count;
            return called_count == 1 ? window : window2;
        };

    windows->register_window("Triggers", creator);
    windows->create("Triggers");
    windows->create("Triggers");

    ASSERT_EQ(called_count, 2);
}

TEST(Windows, ShortcutsRegistered)
{
    auto shortcuts = mock_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut(false, VK_F11)).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    EXPECT_CALL(*shortcuts, add_shortcut(true, 'D')).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    EXPECT_CALL(*shortcuts, add_shortcut(true, 'I')).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    EXPECT_CALL(*shortcuts, add_shortcut(true, 'K')).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    EXPECT_CALL(*shortcuts, add_shortcut(true, 'L')).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    EXPECT_CALL(*shortcuts, add_shortcut(true, 'M')).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    EXPECT_CALL(*shortcuts, add_shortcut(true, 'P')).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    EXPECT_CALL(*shortcuts, add_shortcut(true, 'R')).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    EXPECT_CALL(*shortcuts, add_shortcut(true, 'S')).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    EXPECT_CALL(*shortcuts, add_shortcut(true, 'T')).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    register_test_module().with_shortcuts(shortcuts).build();
}
