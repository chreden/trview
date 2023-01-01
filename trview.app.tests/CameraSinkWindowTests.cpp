#include <trview.app/Windows/CameraSink/CameraSinkWindow.h>
#include <trview.app/Mocks/Elements/ICameraSink.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IClipboard> clipboard{ mock_shared<MockClipboard>() };

            std::unique_ptr<CameraSinkWindow> build()
            {
                return std::make_unique<CameraSinkWindow>(clipboard);
            }
        };

        return test_module{};
    }
}

TEST(CameraSinkWindow, SelectedRaised)
{
    auto window = register_test_module().build();

    std::shared_ptr<ICameraSink> raised;
    auto token = window->on_camera_sink_selected += [&raised](const auto& camera_sink) { raised = camera_sink.lock(); };

    auto camera_sink1 = mock_shared<MockCameraSink>()->with_number(0);
    auto camera_sink2 = mock_shared<MockCameraSink>()->with_number(1);
    window->set_camera_sinks({ camera_sink1, camera_sink2 });

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element_with_hover(imgui.id("Camera/Sink 0")
        .push_child(CameraSinkWindow::Names::list_panel)
        .push(CameraSinkWindow::Names::camera_sink_list).id("1##1"));

    ASSERT_EQ(raised, camera_sink2);
}

TEST(CameraSinkWindow, SelectedNotRaisedWhenSyncOff)
{
    auto window = register_test_module().build();

    std::shared_ptr<ICameraSink> raised;
    auto token = window->on_camera_sink_selected += [&raised](const auto& camera_sink) { raised = camera_sink.lock(); };

    auto camera_sink1 = mock_shared<MockCameraSink>()->with_number(0);
    auto camera_sink2 = mock_shared<MockCameraSink>()->with_number(1);
    window->set_camera_sinks({ camera_sink1, camera_sink2 });

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Camera/Sink 0")
        .push_child(CameraSinkWindow::Names::list_panel)
        .id(CameraSinkWindow::Names::sync));

    imgui.click_element_with_hover(imgui.id("Camera/Sink 0")
        .push_child(CameraSinkWindow::Names::list_panel)
        .push(CameraSinkWindow::Names::camera_sink_list).id("1##1"));

    ASSERT_FALSE(raised);
}

TEST(CameraSinkWindow, VisibilityRaised)
{
    auto window = register_test_module().build();

    std::optional<std::tuple<std::shared_ptr<ICameraSink>, bool>> raised;
    auto token = window->on_camera_sink_visibility += [&raised](const auto& camera_sink, bool visible) { raised = { camera_sink.lock(), visible }; };

    auto camera_sink1 = mock_shared<MockCameraSink>()->with_number(0);
    auto camera_sink2 = mock_shared<MockCameraSink>()->with_number(1);
    ON_CALL(*camera_sink2, visible).WillByDefault(testing::Return(true));

    window->set_camera_sinks({ camera_sink1, camera_sink2 });

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element_with_hover(imgui.id("Camera/Sink 0")
        .push_child(CameraSinkWindow::Names::list_panel)
        .push(CameraSinkWindow::Names::camera_sink_list).id("##hide-1"));

    ASSERT_TRUE(raised);
    ASSERT_EQ(std::get<0>(raised.value()), camera_sink2);
    ASSERT_EQ(std::get<1>(raised.value()), false);
}

TEST(CameraSinkWindow, TypeChangedRaised)
{
    auto window = register_test_module().build();

    bool raised = false;
    auto token = window->on_camera_sink_type_changed += [&raised]() { raised = true; };

    auto camera_sink = mock_shared<MockCameraSink>()->with_number(0)->with_type(ICameraSink::Type::Camera);

    window->set_camera_sinks({ camera_sink });
    window->set_selected_camera_sink(camera_sink);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Camera/Sink 0")
        .push_child(CameraSinkWindow::Names::details_panel)
        .id(CameraSinkWindow::Names::type));
    imgui.click_element(imgui.id("##Combo_00").id("Sink##type"));

    ASSERT_TRUE(raised);
}

TEST(CameraSinkWindow, TriggerSelectedRaised)
{
    auto window = register_test_module().build();

    std::shared_ptr<ITrigger> raised;
    auto token = window->on_trigger_selected += [&raised](const auto& trigger) { raised = trigger.lock(); };

    auto trigger1 = mock_shared<MockTrigger>();
    auto trigger2 = mock_shared<MockTrigger>();
    ON_CALL(*trigger2, number).WillByDefault(testing::Return(1));

    auto camera_sink = mock_shared<MockCameraSink>()
        ->with_number(0)
        ->with_triggers({ trigger1, trigger2 });

    window->set_camera_sinks({ camera_sink });
    window->set_selected_camera_sink(camera_sink);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element_with_hover(imgui.id("Camera/Sink 0")
        .push_child(CameraSinkWindow::Names::details_panel)
        .push(CameraSinkWindow::Names::triggers_list).id("1"));

    ASSERT_EQ(raised, trigger2);
}

TEST(CameraSinkWindow, CameraSinkListFilteredWhenRoomSetAndTrackRoomEnabled)
{
    auto window = register_test_module().build();
    auto camera_sink1 = mock_shared<MockCameraSink>()->with_number(0)->with_room(56);
    auto camera_sink2 = mock_shared<MockCameraSink>()->with_number(1)->with_room(78);
    window->set_camera_sinks({ camera_sink1, camera_sink2 });
    window->set_current_room(78);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Camera/Sink 0")
        .push_child(CameraSinkWindow::Names::list_panel)
        .id(CameraSinkWindow::Names::track_room));

    imgui.reset();
    imgui.render();

    ASSERT_FALSE(imgui.element_present(imgui.id("Camera/Sink 0")
        .push_child(CameraSinkWindow::Names::list_panel)
        .push(CameraSinkWindow::Names::camera_sink_list).id("0##0")));
    ASSERT_TRUE(imgui.element_present(imgui.id("Camera/Sink 0")
        .push_child(CameraSinkWindow::Names::list_panel)
        .push(CameraSinkWindow::Names::camera_sink_list).id("1##1")));
}