#include <trview.app/Windows/CameraSink/CameraSinkWindowManager.h>
#include <trview.app/Mocks/Windows/ICameraSinkWindow.h>
#include <trview.app/Mocks/Elements/ICameraSink.h>
#include <trview.app/Resources/resource.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            Window window{ create_test_window(L"CameraSinkWindowManagerTests") };
            ICameraSinkWindow::Source window_source{ [](auto&&...) { return mock_shared<MockCameraSinkWindow>(); } };

            test_module& with_window_source(const ICameraSinkWindow::Source& source)
            {
                this->window_source = source;
                return *this;
            }

            std::unique_ptr<CameraSinkWindowManager> build()
            {
                return std::make_unique<CameraSinkWindowManager>(window, window_source);
            }
        };

        return test_module{};
    }
}

TEST(CameraSinkWindowManager, WindowCreatedOnCommand)
{
    auto window = mock_shared<MockCameraSinkWindow>();
    bool raised = false;
    auto manager = register_test_module()
        .with_window_source([&]() { raised = true; return window; })
        .build();

    manager->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_CAMERA_SINK, 0), 0);

    ASSERT_TRUE(raised);
}

TEST(CameraSinkWindowManager, WindowCreated)
{
    auto window = mock_shared<MockCameraSinkWindow>();
    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

    EXPECT_CALL(*window, set_current_room).Times(1);
    EXPECT_CALL(*window, set_selected_camera_sink).Times(1);
    EXPECT_CALL(*window, set_camera_sinks).Times(1);
    EXPECT_CALL(*window, set_triggers).Times(1);
    manager->create_window();
}

TEST(CameraSinkWindowManager, CameraSinksForwarded)
{
    auto window = mock_shared<MockCameraSinkWindow>();
    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

    manager->create_window();
    EXPECT_CALL(*window, set_camera_sinks).Times(1);
    manager->set_camera_sinks({});
}

TEST(CameraSinkWindowManager, SelectedCameraSinkForwarded)
{
    auto window = mock_shared<MockCameraSinkWindow>();
    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

    manager->create_window();
    EXPECT_CALL(*window, set_selected_camera_sink).Times(1);
    manager->set_selected_camera_sink({});
}

TEST(CameraSinkWindowManager, RoomForwarded)
{
    auto window = mock_shared<MockCameraSinkWindow>();
    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

    manager->create_window();
    EXPECT_CALL(*window, set_current_room).Times(1);
    manager->set_room(0);
}

TEST(CameraSinkWindowManager, TriggersForwarded)
{
    auto window = mock_shared<MockCameraSinkWindow>();
    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

    manager->create_window();
    EXPECT_CALL(*window, set_triggers).Times(1);
    manager->set_triggers({});
}

TEST(CameraSinkWindowManager, SelectedSinkRaised)
{
    auto window = mock_shared<MockCameraSinkWindow>();
    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

    std::shared_ptr<ICameraSink> raised;
    auto token = manager->on_camera_sink_selected += [&](auto sink)
    {
        raised = sink.lock();
    };

    manager->create_window();

    auto sink = mock_shared<MockCameraSink>();
    manager->on_camera_sink_selected(sink);

    ASSERT_TRUE(raised);
    ASSERT_EQ(raised, sink);
}

TEST(CameraSinkWindowManager, SinkVisibilityRaised)
{
    auto window = mock_shared<MockCameraSinkWindow>();
    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

    std::optional<std::tuple<std::shared_ptr<ICameraSink>, bool>> raised;
    auto token = manager->on_camera_sink_visibility += [&](auto sink, auto visibility)
    {
        raised = { sink.lock(), visibility };
    };

    manager->create_window();

    auto sink = mock_shared<MockCameraSink>();
    manager->on_camera_sink_visibility(sink, true);

    ASSERT_TRUE(raised);
    ASSERT_EQ(std::get<0>(raised.value()), sink);
    ASSERT_EQ(std::get<1>(raised.value()), true);
}

TEST(CameraSinkWindowManager, TriggerSelectedRaised)
{
    auto window = mock_shared<MockCameraSinkWindow>();
    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

    std::shared_ptr<ITrigger> raised;
    auto token = manager->on_trigger_selected += [&](auto trigger)
    {
        raised = trigger.lock();
    };

    manager->create_window();

    auto trigger = mock_shared<MockTrigger>();
    manager->on_trigger_selected(trigger);

    ASSERT_TRUE(raised);
    ASSERT_EQ(raised, trigger);
}

TEST(CameraSinkWindowManager, CameraSinkTypeChangedRaised)
{
    auto window = mock_shared<MockCameraSinkWindow>();
    auto manager = register_test_module()
        .with_window_source([&]() { return window; })
        .build();

    bool raised = false;
    auto token = manager->on_camera_sink_type_changed += [&]()
    {
        raised = true;
    };

    manager->create_window();
    
    manager->on_camera_sink_type_changed();

    ASSERT_TRUE(raised);
}

