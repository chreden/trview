#include <trview.app/Windows/ItemsWindow.h>
#include <trview.app/Elements/Types.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using namespace DirectX::SimpleMath;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IClipboard> clipboard{ mock_shared<MockClipboard>() };

            std::unique_ptr<ItemsWindow> build()
            {
                return std::make_unique<ItemsWindow>(clipboard);
            }

            test_module& with_clipboard(const std::shared_ptr<IClipboard>& clipboard)
            {
                this->clipboard = clipboard;
                return *this;
            }
        };

        return test_module {};
    }
}

TEST(ItemsWindow, AddToRouteEventRaised)
{
    auto window = register_test_module().build();

    std::shared_ptr<IItem> raised_item;
    auto token = window->on_add_to_route += [&raised_item](const auto& item) { raised_item = item.lock(); };

    std::vector<std::shared_ptr<MockItem>> items
    {
        mock_shared<MockItem>()->with_number(0),
        mock_shared<MockItem>()->with_number(1)
    };
    window->set_items({ std::from_range, items });
    window->set_selected_item(items[1]);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Items 0")
        .push_child(ItemsWindow::Names::details_panel)
        .id(ItemsWindow::Names::add_to_route_button));
    ASSERT_TRUE(raised_item);
    ASSERT_EQ(raised_item, items[1]);
}

TEST(ItemsWindow, ItemSelectedNotRaisedWhenSyncItemDisabled)
{
    auto window = register_test_module().build();

    std::shared_ptr<IItem> raised_item;
    auto token = window->on_item_selected += [&raised_item](const auto& item) { raised_item = item.lock(); };

    std::vector<std::shared_ptr<MockItem>> items
    {
        mock_shared<MockItem>()->with_number(0),
        mock_shared<MockItem>()->with_number(1)
    };
    window->set_items({ std::from_range, items });

    TestImgui imgui([&]() { window->render(); });

    imgui.click_element(imgui.id("Items 0")
        .push_child(ItemsWindow::Names::item_list_panel)
        .id(ItemsWindow::Names::sync_item));
    imgui.click_element_with_hover(imgui.id("Items 0")
        .push_child(ItemsWindow::Names::item_list_panel)
        .push(ItemsWindow::Names::items_list).id("1##1"));

    ASSERT_FALSE(raised_item);
}

TEST(ItemsWindow, ItemSelectedRaisedWhenSyncItemEnabled)
{
    auto window = register_test_module().build();

    std::shared_ptr<IItem> raised_item;
    auto token = window->on_item_selected += [&raised_item](const auto& item) { raised_item = item.lock(); };

    std::vector<std::shared_ptr<MockItem>> items
    {
        mock_shared<MockItem>()->with_number(0),
        mock_shared<MockItem>()->with_number(1)
    };
    window->set_items({ std::from_range, items });

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element_with_hover(imgui.id("Items 0")
        .push_child(ItemsWindow::Names::item_list_panel)
        .push(ItemsWindow::Names::items_list).id("1##1"));

    ASSERT_TRUE(raised_item);
    ASSERT_EQ(raised_item, items[1]);

    const auto from_window = window->selected_item().lock();
    ASSERT_TRUE(from_window);
    ASSERT_EQ(from_window, items[1]);
}

TEST(ItemsWindow, ItemVisibilityRaised)
{
    auto window = register_test_module().build();

    std::optional<std::tuple<std::shared_ptr<IItem>, bool>> raised_item;
    auto token = window->on_item_visibility += [&raised_item](const auto& item, bool state) { raised_item = { item.lock(), state }; };

    std::vector<std::shared_ptr<MockItem>> items
    {
        mock_shared<MockItem>()->with_number(0)->with_visible(true),
        mock_shared<MockItem>()->with_number(1)->with_visible(true)
    };
    window->set_items({ std::from_range, items });

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element_with_hover(imgui.id("Items 0")
        .push_child(ItemsWindow::Names::item_list_panel)
        .push(ItemsWindow::Names::items_list).id("##hide-1"));

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_FALSE(std::get<1>(raised_item.value()));
    ASSERT_EQ(std::get<0>(raised_item.value()), items[1]);
}

TEST(ItemsWindow, ItemsListNotFilteredWhenRoomSetAndTrackRoomDisabled)
{
    auto window = register_test_module().build();

    std::shared_ptr<IItem> raised_item;
    auto token = window->on_item_selected += [&raised_item](const auto& item) { raised_item = item.lock(); };

    std::vector<std::shared_ptr<MockItem>> items
    {
        mock_shared<MockItem>()->with_number(0)->with_room(mock_shared<MockRoom>()->with_number(55)),
        mock_shared<MockItem>()->with_number(1)->with_room(mock_shared<MockRoom>()->with_number(78))
    };
    window->set_items({ std::from_range, items });
    window->set_current_room(78);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element_with_hover(imgui.id("Items 0")
        .push_child(ItemsWindow::Names::item_list_panel)
        .push(ItemsWindow::Names::items_list).id("0##0"));

    ASSERT_TRUE(raised_item);
    ASSERT_EQ(raised_item, items[0]);
}

TEST(ItemsWindow, ItemsListFilteredWhenRoomSetAndTrackRoomEnabled)
{
    auto window = register_test_module().build();

    std::shared_ptr<IItem> raised_item;
    auto token = window->on_item_selected += [&raised_item](const auto& item) { raised_item = item.lock(); };

    std::vector<std::shared_ptr<MockItem>> items
    {
        mock_shared<MockItem>()->with_number(0)->with_room(mock_shared<MockRoom>()->with_number(55)),
        mock_shared<MockItem>()->with_number(1)->with_room(mock_shared<MockRoom>()->with_number(78))
    };
    window->set_items({ std::from_range, items });
    window->set_current_room(78);

    TestImgui imgui([&]() { window->render(); });

    imgui.click_element(imgui.id("Items 0").push_child(ItemsWindow::Names::item_list_panel).id(Track<>::Names::Button));
    imgui.click_element(imgui.popup_id(imgui.id("Items 0").push_child(ItemsWindow::Names::item_list_panel), Track<>::Names::Popup).id("Room"));
    imgui.press_key(ImGuiKey_Escape);

    imgui.click_element_with_hover(imgui.id("Items 0")
        .push_child(ItemsWindow::Names::item_list_panel)
        .push(ItemsWindow::Names::items_list).id("1##1"));

    ASSERT_TRUE(raised_item);
    ASSERT_EQ(raised_item, items[1]);
}

TEST(ItemsWindow, ItemsListPopulatedOnSet)
{
    auto window = register_test_module().build();

    std::vector<std::shared_ptr<MockItem>> items
    {
        mock_shared<MockItem>()->with_number(0),
        mock_shared<MockItem>()->with_number(1)
    };
    window->set_items({ std::from_range, items });

    TestImgui imgui([&]() { window->render(); });

    for (std::size_t i = 0; i < items.size(); ++i)
    {
        ASSERT_TRUE(imgui.element_present(imgui.id("Items 0")
            .push_child(ItemsWindow::Names::item_list_panel)
            .push(ItemsWindow::Names::items_list).id(std::to_string(i) + "##" + std::to_string(i))));
    }
}

TEST(ItemsWindow, ItemsListUpdatedWhenFiltered)
{
    auto window = register_test_module().build();

    std::vector<std::shared_ptr<MockItem>> items
    {
        mock_shared<MockItem>()->with_number(0)->with_room(mock_shared<MockRoom>()->with_number(55)),
        mock_shared<MockItem>()->with_number(1)->with_room(mock_shared<MockRoom>()->with_number(78))
    };
    window->set_items({ std::from_range, items });
    window->set_current_room(78);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Items 0").push_child(ItemsWindow::Names::item_list_panel).id(Track<>::Names::Button));
    imgui.click_element(imgui.popup_id(imgui.id("Items 0").push_child(ItemsWindow::Names::item_list_panel), Track<>::Names::Popup).id("Room"));
    imgui.reset();
    imgui.render();

    ASSERT_FALSE(imgui.element_present(
        imgui.id("Items 0").push_child(ItemsWindow::Names::item_list_panel)
        .push(ItemsWindow::Names::items_list).id("0##0")));
    ASSERT_TRUE(imgui.element_present(
        imgui.id("Items 0").push_child(ItemsWindow::Names::item_list_panel)
        .push(ItemsWindow::Names::items_list).id("1##1")));
}

TEST(ItemsWindow, ItemsListUpdatedWhenNotFiltered)
{
    auto window = register_test_module().build();

    std::vector<std::shared_ptr<MockItem>> items
    {
        mock_shared<MockItem>()->with_number(0)->with_room(mock_shared<MockRoom>()->with_number(55))->with_visible(true),
        mock_shared<MockItem>()->with_number(1)->with_room(mock_shared<MockRoom>()->with_number(78))->with_visible(true)
    };
    window->set_items({ std::from_range, items });
    TestImgui imgui([&]() { window->render(); });

    ASSERT_TRUE(imgui.element_present(
        imgui.id("Items 0").push_child(ItemsWindow::Names::item_list_panel)
        .push(ItemsWindow::Names::items_list).id("1##1")));

    ASSERT_FALSE(imgui.status_flags(
        imgui.id("Items 0").push_child(ItemsWindow::Names::item_list_panel)
        .push(ItemsWindow::Names::items_list).id("##hide-1")) & ImGuiItemStatusFlags_Checked);

    ON_CALL(*items[1], visible).WillByDefault(testing::Return(false));

    imgui.reset();
    imgui.render();

    ASSERT_TRUE(imgui.status_flags(
        imgui.id("Items 0").push_child(ItemsWindow::Names::item_list_panel)
        .push(ItemsWindow::Names::items_list).id("##hide-1")) & ImGuiItemStatusFlags_Checked);
}

TEST(ItemsWindow, TriggersLoadedForItem)
{
    auto window = register_test_module().build();

    auto trigger1 = mock_shared<MockTrigger>()->with_number(0);
    auto trigger2 = mock_shared<MockTrigger>()->with_number(1);
    std::vector<std::shared_ptr<MockItem>> items
    {
        mock_shared<MockItem>()->with_number(0),
        mock_shared<MockItem>()->with_number(1)->with_triggers({ trigger1, trigger2 })
    };
    window->set_items({ std::from_range, items });
    window->set_triggers({ trigger1, trigger2 });

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element_with_hover(imgui.id("Items 0")
        .push_child(ItemsWindow::Names::item_list_panel)
        .push(ItemsWindow::Names::items_list).id("1##1"));

    ASSERT_TRUE(imgui.element_present(
        imgui.id("Items 0").push_child(ItemsWindow::Names::details_panel)
        .push(ItemsWindow::Names::triggers_list).id("0")));
    ASSERT_TRUE(imgui.element_present(
        imgui.id("Items 0").push_child(ItemsWindow::Names::details_panel)
        .push(ItemsWindow::Names::triggers_list).id("1")));
}

TEST(ItemsWindow, TriggerSelectedEventRaised)
{
    auto window = register_test_module().build();

    std::optional<std::weak_ptr<ITrigger>> raised_trigger;
    auto token = window->on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger = mock_shared<MockTrigger>();
    std::vector<std::shared_ptr<MockItem>> items
    {
        mock_shared<MockItem>()->with_number(0),
        mock_shared<MockItem>()->with_number(1)->with_triggers({ trigger })
    };
    window->set_items({ std::from_range, items });
    window->set_triggers({ trigger });

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element_with_hover(imgui.id("Items 0")
        .push_child(ItemsWindow::Names::item_list_panel)
        .push(ItemsWindow::Names::items_list).id("1##1"));

    imgui.click_element_with_hover(imgui.id("Items 0")
        .push_child(ItemsWindow::Names::details_panel)
        .push(ItemsWindow::Names::triggers_list).id("0"));

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value().lock(), trigger);
}

TEST(ItemsWindow, ClickStatShowsBubbleAndCopies)
{
    auto clipboard = mock_shared<MockClipboard>();
    EXPECT_CALL(*clipboard, write).Times(1);

    auto window = register_test_module().with_clipboard(clipboard).build();

    std::vector<std::shared_ptr<MockItem>> items
    {
        mock_shared<MockItem>()
    };
    window->set_items({ std::from_range, items });
    window->set_selected_item(items[0]);

    TestImgui imgui([&]() { window->render(); });

    ASSERT_EQ(imgui.find_window("##Tooltip_00"), nullptr);
    const auto id = imgui.id("Items 0")
        .push_child(ItemsWindow::Names::details_panel)
        .push(ItemsWindow::Names::item_stats)
        .id("Position");
    imgui.click_element(id);
    ASSERT_NE(imgui.find_window("##Tooltip_00"), nullptr);
}
