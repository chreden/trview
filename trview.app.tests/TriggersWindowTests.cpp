#include <trview.app/Windows/TriggersWindow.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/ShaderStorage.h>
#include <trview.graphics/mocks/IFontFactory.h>
#include <trview.graphics/mocks/IFont.h>
#include <trview.ui/Button.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Dropdown.h>
#include <trview.ui/Listbox.h>
#include <trview.app/Elements/Types.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::graphics;

TEST(TriggersWindow, TriggerSelectedRaisedWhenSyncTriggerEnabled)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    TriggersWindow window(device, shader_storage, font_factory, create_test_window(L"TriggersWindowTests"));

    std::optional<const Trigger*> raised_trigger;
    auto token = window.on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Object, 1 }} });
    auto trigger2 = std::make_unique<Trigger>(1, 0, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Camera, 1 }} });
    std::vector<Trigger*> triggers{ trigger1.get(), trigger2.get() };
    window.set_triggers(triggers);

    auto list = window.root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value(), trigger2.get());
}

TEST(TriggersWindow, TriggerSelectedNotRaisedWhenSyncTriggerDisabled)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    TriggersWindow window(device, shader_storage, font_factory, create_test_window(L"TriggersWindowTests"));

    std::optional<const Trigger*> raised_trigger;
    auto token = window.on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Object, 1 }} });
    auto trigger2 = std::make_unique<Trigger>(1, 0, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Camera, 1 }} });
    std::vector<Trigger*> triggers{ trigger1.get(), trigger2.get() };
    window.set_triggers(triggers);

    auto sync = window.root_control()->find<ui::Checkbox>(TriggersWindow::Names::sync_trigger_checkbox);
    ASSERT_NE(sync, nullptr);
    sync->clicked(Point());

    auto list = window.root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_FALSE(raised_trigger.has_value());
}

TEST(TriggersWindow, TriggersListNotFilteredWhenTrackRoomDisabled)
{
    FAIL();
}

TEST(TriggersWindow, TriggersListFilteredWhenTrackRoomEnabled)
{
    FAIL();
}

TEST(TriggersWindow, TriggersListFilteredByCommand)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    TriggersWindow window(device, shader_storage, font_factory, create_test_window(L"TriggersWindowTests"));

    std::optional<const Trigger*> raised_trigger;
    auto token = window.on_add_to_route += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Object, 1 }} });
    auto trigger2 = std::make_unique<Trigger>(1, 0, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Camera, 1 }} });
    std::vector<Trigger*> triggers{ trigger1.get(), trigger2.get() };
    window.set_triggers(triggers);

    auto list = window.root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);
    ASSERT_EQ(list->items().size(), 2);
    ASSERT_EQ(list->items()[0].value(L"#"), L"0");
    ASSERT_EQ(list->items()[1].value(L"#"), L"1");

    auto dropdown = window.root_control()->find<ui::Dropdown>(TriggersWindow::Names::filter_dropdown);
    ASSERT_NE(dropdown, nullptr);

    auto dropdown_button = dropdown->find<ui::Button>(ui::Dropdown::Names::dropdown_button);
    ASSERT_NE(dropdown_button, nullptr);
    dropdown_button->clicked(Point());

    auto dropdown_list = dropdown->dropdown_listbox();
    ASSERT_NE(dropdown_list, nullptr);

    auto row = dropdown_list->find<ui::Control>(ui::Listbox::Names::row_name_format + "3");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Name");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());
    
    ASSERT_EQ(list->items().size(), 1);
    ASSERT_EQ(list->items()[0].value(L"#"), L"1");
}

TEST(TriggersWindow, AddToRouteEventRaised)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    TriggersWindow window(device, shader_storage, font_factory, create_test_window(L"TriggersWindowTests"));

    std::optional<const Trigger*> raised_trigger;
    auto token = window.on_add_to_route += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{});
    std::vector<Trigger*> triggers{ trigger1.get() };

    window.set_triggers(triggers);
    window.set_selected_trigger(trigger1.get());

    auto button = window.root_control()->find<ui::Button>(TriggersWindow::Names::add_to_route_button);
    ASSERT_NE(button, nullptr);
    button->clicked(Point());

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value(), trigger1.get());
}

TEST(TriggersWindow, TriggerVisibilityRaised)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    TriggersWindow window(device, shader_storage, font_factory, create_test_window(L"TriggersWindowTests"));

    std::optional<std::tuple<const Trigger*, bool>> raised_trigger;
    auto token = window.on_trigger_visibility += [&raised_trigger](const auto& trigger, bool state) { raised_trigger = { trigger, state }; };

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{});
    std::vector<Trigger*> triggers{ trigger1.get() };
    window.set_triggers(triggers);

    auto list = window.root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Checkbox>(ui::Listbox::Row::Names::cell_name_format + "Hide");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(std::get<0>(raised_trigger.value()), trigger1.get());
    ASSERT_FALSE(std::get<1>(raised_trigger.value()));
}

TEST(TriggersWindow, ItemSelectedRaised)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    TriggersWindow window(device, shader_storage, font_factory, create_test_window(L"TriggersWindowTests"));

    std::optional<Item> raised_item;
    auto token = window.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Object, 1 }}});
    std::vector<Trigger*> triggers{ trigger1.get() };
    window.set_items(items);
    window.set_triggers(triggers);
    window.set_selected_trigger(trigger1.get());

    auto list = window.root_control()->find<ui::Listbox>(TriggersWindow::Names::trigger_commands_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Index");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 1);
}

TEST(TriggersWindow, ClearSelectedTriggerClearsSelection)
{
    FAIL();
}

TEST(TriggersWindow, TriggerDetailsLoadedForTrigger)
{
    FAIL();
}

TEST(TriggersWindow, SelectionSurvivesFiltering)
{
    FAIL();
}

TEST(TriggersWindow, FlipmapsFiltersAllFlipTriggers)
{
    FAIL();
}
