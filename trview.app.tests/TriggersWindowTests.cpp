#include <trview.app/Windows/TriggersWindow.h>
#include <trview.app/Elements/Types.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using testing::Return;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IClipboard> clipboard{ mock_shared<MockClipboard>() };

            std::unique_ptr<TriggersWindow> build()
            {
                return std::make_unique<TriggersWindow>(clipboard);
            }
        };

        return test_module {};
    }
}

TEST(TriggersWindow, TriggerSelectedRaisedWhenSyncTriggerEnabled)
{
    auto window = register_test_module().build();

    std::optional<std::weak_ptr<ITrigger>> raised_trigger;
    auto token = window->on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger1 = mock_shared<MockTrigger>()->with_number(0);
    auto trigger2 = mock_shared<MockTrigger>()->with_number(1);
    window->set_triggers({ trigger1, trigger2 });

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element_with_hover(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list).id("1##1"));

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value().lock(), trigger2);

    const auto from_window = window->selected_trigger();
    ASSERT_NE(from_window.lock(), nullptr);
    ASSERT_EQ(from_window.lock()->number(), 1);
}

TEST(TriggersWindow, TriggerSelectedNotRaisedWhenSyncTriggerDisabled)
{
    auto window = register_test_module().build();

    std::optional<std::weak_ptr<ITrigger>> raised_trigger;
    auto token = window->on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger1 = mock_shared<MockTrigger>()->with_number(0);
    auto trigger2 = mock_shared<MockTrigger>()->with_number(1);
    window->set_triggers({ trigger1, trigger2 });

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .id(TriggersWindow::Names::sync_trigger));

    imgui.click_element_with_hover(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list).id("1##1"));

    ASSERT_FALSE(raised_trigger.has_value());
}

TEST(TriggersWindow, TriggersListNotFilteredWhenRoomSetAndTrackRoomDisabled)
{
    auto window = register_test_module().build();

    auto trigger1 = mock_shared<MockTrigger>()->with_number(0);
    auto trigger2 = mock_shared<MockTrigger>()->with_number(1);
    window->set_triggers({ trigger1, trigger2 });
    window->set_current_room(78);

    TestImgui imgui([&]() { window->render(); });

    ASSERT_TRUE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("0##0")));
    ASSERT_TRUE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("1##1")));
}

TEST(TriggersWindow, TriggersListFilteredWhenRoomSetAndTrackRoomEnabled)
{
    auto window = register_test_module().build();

    auto trigger1 = mock_shared<MockTrigger>()->with_number(0)->with_room(55);
    auto trigger2 = mock_shared<MockTrigger>()->with_number(1)->with_room(78);
    window->set_triggers({ trigger1, trigger2 });
    window->set_current_room(78);

    TestImgui imgui([&]() { window->render(); });
    ASSERT_TRUE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("0##0")));
    ASSERT_TRUE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("1##1")));

    imgui.reset();
    imgui.click_element(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .id(TriggersWindow::Names::track_room));

    ASSERT_FALSE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("0##0")));
    ASSERT_TRUE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("1##1")));
}

TEST(TriggersWindow, TriggersListFilteredByCommand)
{
    auto window = register_test_module().build();
    auto trigger1 = mock_shared<MockTrigger>()->with_commands({ Command(0, TriggerCommandType::Object, 1) });
    auto trigger2 = mock_shared<MockTrigger>()->with_number(1)->with_commands({ Command(0, TriggerCommandType::Camera, 1) });
    std::vector<std::weak_ptr<ITrigger>> triggers{ trigger1, trigger2 };
    window->set_triggers(triggers);

    TestImgui imgui([&]() { window->render(); });
    ASSERT_TRUE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("0##0")));
    ASSERT_TRUE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("1##1")));

    imgui.click_element(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .id(TriggersWindow::Names::command_filter));
    imgui.click_element(imgui.id("##Combo_00").id("Camera"));

    imgui.reset();
    imgui.render();

    ASSERT_FALSE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("0##0")));
    ASSERT_TRUE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("1##1")));
}

TEST(TriggersWindow, AddToRouteEventRaised)
{
    auto window = register_test_module().build();

    std::optional<std::weak_ptr<ITrigger>> raised_trigger;
    auto token = window->on_add_to_route += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger = mock_shared<MockTrigger>();
    std::vector<std::weak_ptr<ITrigger>> triggers{ trigger };

    window->set_triggers(triggers);
    window->set_selected_trigger(trigger);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::details_panel)
        .id(TriggersWindow::Names::add_to_route));

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value().lock(), trigger);
}

TEST(TriggersWindow, TriggerVisibilityRaised)
{
    auto window = register_test_module().build();

    std::optional<std::tuple<std::weak_ptr<ITrigger>, bool>> raised_trigger;
    auto token = window->on_trigger_visibility += [&raised_trigger](const auto& trigger, bool state) { raised_trigger = { trigger, state }; };

    auto trigger = mock_shared<MockTrigger>()->with_visible(true);
    std::vector<std::weak_ptr<ITrigger>> triggers{ trigger };
    window->set_triggers(triggers);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element_with_hover(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list).id("##hide-0"));

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(std::get<0>(raised_trigger.value()).lock(), trigger);
    ASSERT_FALSE(std::get<1>(raised_trigger.value()));
}

TEST(TriggersWindow, ItemSelectedRaised)
{
    auto window = register_test_module().build();

    std::optional<Item> raised_item;
    auto token = window->on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    std::vector<Item> items
    {
        Item(0, 0, 0, "Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, "Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };

    auto trigger = mock_shared<MockTrigger>()->with_commands({ Command(0, TriggerCommandType::Object, 1) });
    window->set_items(items);
    window->set_triggers({ trigger });
    window->set_selected_trigger(trigger);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element_with_hover(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::details_panel)
        .push(TriggersWindow::Names::commands_list).id("1##1"));

    auto selected = window->selected_trigger().lock();
    ASSERT_NE(selected, nullptr);
    ASSERT_EQ(selected, trigger);
}

TEST(TriggersWindow, FlipmapsFiltersAllFlipTriggers)
{
    auto window = register_test_module().build();
    auto trigger1 = mock_shared<MockTrigger>()->with_number(0)->with_commands({ Command(0, TriggerCommandType::FlipOff, 0) });
    auto trigger2 = mock_shared<MockTrigger>()->with_number(1)->with_commands({ Command(0, TriggerCommandType::FlipOn, 0) });
    auto trigger3 = mock_shared<MockTrigger>()->with_number(2)->with_commands({ Command(0, TriggerCommandType::FlipMap, 0) });
    auto trigger4 = mock_shared<MockTrigger>()->with_number(3);
    window->set_triggers({ trigger1, trigger2, trigger3, trigger4 });

    TestImgui imgui([&]() { window->render(); });
    ASSERT_TRUE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("0##0")));
    ASSERT_TRUE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("1##1")));
    ASSERT_TRUE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("2##2")));
    ASSERT_TRUE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("3##3")));

    imgui.click_element(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .id(TriggersWindow::Names::command_filter));
    imgui.click_element(imgui.id("##Combo_00").id("Flipmaps"));
    imgui.reset();
    imgui.render();

    ASSERT_TRUE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("0##0")));
    ASSERT_TRUE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("1##1")));
    ASSERT_TRUE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("2##2")));
    ASSERT_FALSE(imgui.element_present(imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::trigger_list_panel)
        .push(TriggersWindow::Names::triggers_list)
        .id("3##3")));
}

TEST(TriggersWindow, ClickStatShowsBubble)
{
    auto window = register_test_module().build();

    auto trigger1 = mock_shared<MockTrigger>()->with_number(0)->with_commands({ Command(0, TriggerCommandType::FlipOff, 0) });
    window->set_triggers({ trigger1 });
    window->set_selected_trigger(trigger1);

    TestImgui imgui([&]() { window->render(); });

    ASSERT_EQ(imgui.find_window("##Tooltip_00"), nullptr);
    const auto id = imgui.id("Triggers 0")
        .push_child(TriggersWindow::Names::details_panel)
        .push(TriggersWindow::Names::trigger_stats)
        .id("Position");
    imgui.click_element(id);
    ASSERT_NE(imgui.find_window("##Tooltip_00"), nullptr);
}
