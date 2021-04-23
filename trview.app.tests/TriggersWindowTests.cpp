#include <trview.app/Windows/TriggersWindow.h>
#include <trview.ui.render/Mocks/IRenderer.h>
#include <trview.ui/Button.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Dropdown.h>
#include <trview.ui/Listbox.h>
#include <trview.app/Elements/Types.h>
#include <trview.graphics/mocks/IDeviceWindow.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.ui/Mocks/Input/IInput.h>
#include <external/boost/di.hpp>

using namespace trview;
using namespace trview::tests;
using namespace trview::graphics;
using namespace trview::graphics::mocks;
using namespace trview::ui::mocks;
using namespace trview::ui::render::mocks;
using namespace trview::mocks;

namespace
{
    auto register_test_module()
    {
        using namespace boost;
        return di::make_injector(
            di::bind<IDeviceWindow::Source>.to([&](auto&&) { return [&](auto&&) { return std::make_unique<MockDeviceWindow>(); }; }),
            di::bind<ui::render::IRenderer::Source>.to([&](auto&&) { return [&](auto&&) { return std::make_unique<MockRenderer>(); }; }),
            di::bind<ui::IInput::Source>.to([&](auto&&) { return [&](auto&&, auto&&) { return std::make_unique<MockInput>(); }; }),
            di::bind<Window>.to(create_test_window(L"TriggersWindowTests")),
            di::bind<IClipboard>.to<MockClipboard>(),
            di::bind<TriggersWindow>()
        );
    }
}

TEST(TriggersWindow, TriggerSelectedRaisedWhenSyncTriggerEnabled)
{
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    std::optional<const Trigger*> raised_trigger;
    auto token = window->on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Object, 1 }} }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    auto trigger2 = std::make_unique<Trigger>(1, 0, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Camera, 1 }} }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    std::vector<Trigger*> triggers{ trigger1.get(), trigger2.get() };
    window->set_triggers(triggers);

    auto list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
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
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    std::optional<const Trigger*> raised_trigger;
    auto token = window->on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Object, 1 }} }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    auto trigger2 = std::make_unique<Trigger>(1, 0, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Camera, 1 }} }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    std::vector<Trigger*> triggers{ trigger1.get(), trigger2.get() };
    window->set_triggers(triggers);

    auto sync = window->root_control()->find<ui::Checkbox>(TriggersWindow::Names::sync_trigger_checkbox);
    ASSERT_NE(sync, nullptr);
    sync->clicked(Point());

    auto list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_FALSE(raised_trigger.has_value());
}

TEST(TriggersWindow, TriggersListNotFilteredWhenRoomSetAndTrackRoomDisabled)
{
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    std::optional<Trigger*> raised_trigger;
    auto token = window->on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger1 = std::make_unique<Trigger>(0, 55, 100, 200, TriggerInfo{}, [](auto, auto) { return std::make_unique<MockMesh>(); });
    auto trigger2 = std::make_unique<Trigger>(1, 78, 100, 200, TriggerInfo{}, [](auto, auto) { return std::make_unique<MockMesh>(); });
    window->set_triggers({ trigger1.get(), trigger2.get() });
    window->set_current_room(78);

    auto list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value()->number(), 0);
}

TEST(TriggersWindow, TriggersListFilteredWhenRoomSetAndTrackRoomEnabled)
{
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    std::optional<Trigger*> raised_trigger;
    auto token = window->on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger1 = std::make_unique<Trigger>(0, 55, 100, 200, TriggerInfo{}, [](auto, auto) { return std::make_unique<MockMesh>(); });
    auto trigger2 = std::make_unique<Trigger>(1, 78, 100, 200, TriggerInfo{}, [](auto, auto) { return std::make_unique<MockMesh>(); });
    window->set_triggers({ trigger1.get(), trigger2.get() });
    window->set_current_room(78);

    auto track = window->root_control()->find<ui::Checkbox>(TriggersWindow::Names::track_room_checkbox);
    ASSERT_NE(track, nullptr);
    track->clicked(Point());

    auto list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value()->number(), 1);
}

TEST(TriggersWindow, TriggersListFilteredByCommand)
{
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    std::optional<const Trigger*> raised_trigger;
    auto token = window->on_add_to_route += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Object, 1 }} }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    auto trigger2 = std::make_unique<Trigger>(1, 0, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Camera, 1 }} }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    std::vector<Trigger*> triggers{ trigger1.get(), trigger2.get() };
    window->set_triggers(triggers);

    auto list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);
    ASSERT_EQ(list->items().size(), 2);
    ASSERT_EQ(list->items()[0].value(L"#"), L"0");
    ASSERT_EQ(list->items()[1].value(L"#"), L"1");

    auto dropdown = window->root_control()->find<ui::Dropdown>(TriggersWindow::Names::filter_dropdown);
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
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    std::optional<const Trigger*> raised_trigger;
    auto token = window->on_add_to_route += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{}, [](auto, auto) { return std::make_unique<MockMesh>(); });
    std::vector<Trigger*> triggers{ trigger1.get() };

    window->set_triggers(triggers);
    window->set_selected_trigger(trigger1.get());

    auto button = window->root_control()->find<ui::Button>(TriggersWindow::Names::add_to_route_button);
    ASSERT_NE(button, nullptr);
    button->clicked(Point());

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value(), trigger1.get());
}

TEST(TriggersWindow, TriggerVisibilityRaised)
{
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    std::optional<std::tuple<const Trigger*, bool>> raised_trigger;
    auto token = window->on_trigger_visibility += [&raised_trigger](const auto& trigger, bool state) { raised_trigger = { trigger, state }; };

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{}, [](auto, auto) { return std::make_unique<MockMesh>(); });
    std::vector<Trigger*> triggers{ trigger1.get() };
    window->set_triggers(triggers);

    auto list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
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
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    std::optional<Item> raised_item;
    auto token = window->on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Object, 1 }}}, [](auto, auto) { return std::make_unique<MockMesh>(); });
    std::vector<Trigger*> triggers{ trigger1.get() };
    window->set_items(items);
    window->set_triggers(triggers);
    window->set_selected_trigger(trigger1.get());

    auto list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::trigger_commands_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Index");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 1);

    auto selected = window->selected_trigger();
    ASSERT_NE(selected, nullptr);
    ASSERT_EQ(selected, trigger1.get());
}

TEST(TriggersWindow, SetTriggersLoadsTriggers)
{
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    auto trigger1 = std::make_unique<Trigger>(0, 55, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, {  } }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    auto trigger2 = std::make_unique<Trigger>(1, 78, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Antipad, 0, { { TriggerCommandType::Camera, 0 } } }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    window->set_triggers({ trigger1.get(), trigger2.get() });

    auto list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);
    ASSERT_EQ(list->items().size(), 2);
}

TEST(TriggersWindow, SetTriggerVisiblityUpdatesTrigger)
{
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    auto list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);
    ASSERT_TRUE(list->items().empty());

    auto trigger1 = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{}, [](auto, auto) { return std::make_unique<MockMesh>(); });
    window->set_triggers({ trigger1.get() });
    ASSERT_EQ(list->items().size(), 1);
    ASSERT_EQ(list->items()[0].value(L"Hide"), L"0");

    trigger1->set_visible(false);
    window->update_triggers({ trigger1.get() });
    ASSERT_EQ(list->items()[0].value(L"Hide"), L"1");
}

TEST(TriggersWindow, SetItemsLoadsItems)
{
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Test Object", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
    };
    window->set_items(items);
    auto trigger1 = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Object, 0 }} }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    window->set_triggers({ trigger1.get() });

    window->set_selected_trigger(trigger1.get());

    auto commands_list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::trigger_commands_listbox);
    ASSERT_NE(commands_list, nullptr);

    auto command_items = commands_list->items();
    ASSERT_NE(command_items.size(), 0);
    ASSERT_EQ(command_items[0].value(L"Index"), L"0");
    ASSERT_EQ(command_items[0].value(L"Entity"), L"Test Object");
}

TEST(TriggersWindow, ClearSelectedTriggerClearsSelection)
{
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    std::optional<Trigger*> raised_trigger;
    auto token = window->on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger1 = std::make_unique<Trigger>(0, 55, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, {  } }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    auto trigger2 = std::make_unique<Trigger>(1, 78, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Antipad, 0, { { TriggerCommandType::Camera, 0 } } }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    window->set_triggers({ trigger1.get(), trigger2.get() });

    auto list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(list->selected_item().has_value());
    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value()->number(), 1);

    auto stats_list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::stats_listbox);
    ASSERT_NE(stats_list, nullptr);

    auto stats_items = stats_list->items();
    ASSERT_NE(stats_items.size(), 0);

    auto commands_list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::trigger_commands_listbox);
    ASSERT_NE(commands_list, nullptr);

    auto commands_items = commands_list->items();
    ASSERT_NE(commands_items.size(), 0);

    window->clear_selected_trigger();
    ASSERT_FALSE(list->selected_item().has_value());

    stats_items = stats_list->items();
    ASSERT_EQ(stats_items.size(), 0);

    commands_items = commands_list->items();
    ASSERT_EQ(commands_items.size(), 0);
}

TEST(TriggersWindow, SetTriggersClearsSelection)
{
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    std::optional<Trigger*> raised_trigger;
    auto token = window->on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger1 = std::make_unique<Trigger>(0, 55, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, {  } }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    auto trigger2 = std::make_unique<Trigger>(1, 78, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Antipad, 0, { { TriggerCommandType::Camera, 0 } } }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    window->set_triggers({ trigger1.get(), trigger2.get() });

    auto list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(list->selected_item().has_value());
    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value()->number(), 1);

    auto stats_list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::stats_listbox);
    ASSERT_NE(stats_list, nullptr);

    auto stats_items = stats_list->items();
    ASSERT_NE(stats_items.size(), 0);

    auto commands_list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::trigger_commands_listbox);
    ASSERT_NE(commands_list, nullptr);

    auto commands_items = commands_list->items();
    ASSERT_NE(commands_items.size(), 0);

    window->set_triggers({});
    ASSERT_FALSE(list->selected_item().has_value());

    stats_items = stats_list->items();
    ASSERT_EQ(stats_items.size(), 0);

    commands_items = commands_list->items();
    ASSERT_EQ(commands_items.size(), 0);
}

TEST(TriggersWindow, TriggerDetailsLoadedForTrigger)
{
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    std::optional<Trigger*> raised_trigger;
    auto token = window->on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger1 = std::make_unique<Trigger>(0, 55, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, {  } }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    window->set_triggers({ trigger1.get() });

    auto stats_list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::stats_listbox);
    ASSERT_NE(stats_list, nullptr);
    ASSERT_EQ(stats_list->items().size(), 0);

    window->set_selected_trigger(trigger1.get());
    ASSERT_NE(stats_list->items().size(), 0);
}

TEST(TriggersWindow, SelectionSurvivesFiltering)
{
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    auto trigger1 = std::make_unique<Trigger>(0, 55, 100, 200, TriggerInfo{}, [](auto, auto) { return std::make_unique<MockMesh>(); });
    auto trigger2 = std::make_unique<Trigger>(1, 78, 100, 200, TriggerInfo{}, [](auto, auto) { return std::make_unique<MockMesh>(); });
    window->set_triggers({ trigger1.get(), trigger2.get() });
    window->set_current_room(78);

    auto list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    auto track = window->root_control()->find<ui::Checkbox>(TriggersWindow::Names::track_room_checkbox);
    ASSERT_NE(track, nullptr);
    track->clicked(Point());

    auto now_selected = list->selected_item();
    ASSERT_TRUE(now_selected.has_value());
    ASSERT_EQ(now_selected.value().value(L"#"), L"1");
}

TEST(TriggersWindow, FlipmapsFiltersAllFlipTriggers)
{
    auto window = register_test_module().create<std::unique_ptr<TriggersWindow>>();

    auto trigger1 = std::make_unique<Trigger>(0, 55, 100, 200, TriggerInfo{0,0,0,TriggerType::Trigger, 0, { { TriggerCommandType::FlipOff, 0 } } }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    auto trigger2 = std::make_unique<Trigger>(1, 78, 100, 200, TriggerInfo{0,0,0,TriggerType::Trigger, 0, { { TriggerCommandType::FlipOn, 0 } } }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    auto trigger3 = std::make_unique<Trigger>(2, 78, 100, 200, TriggerInfo{ 0,0,0,TriggerType::Trigger, 0, { { TriggerCommandType::FlipMap, 0 } } }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    auto trigger4 = std::make_unique<Trigger>(3, 78, 100, 200, TriggerInfo{ 0,0,0,TriggerType::Trigger, 0, { { TriggerCommandType::Object, 0 } } }, [](auto, auto) { return std::make_unique<MockMesh>(); });
    window->set_triggers({ trigger1.get(), trigger2.get(), trigger3.get(), trigger4.get() });

    auto list = window->root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);

    auto get_numbers = [&list]()
    {
        std::vector<uint32_t> numbers;
        for (const auto& item : list->items()) { numbers.push_back(std::stoi(item.value(L"#"))); }
        return numbers;
    };

    using ::testing::ElementsAre;
    ASSERT_THAT(get_numbers(), ElementsAre(0, 1, 2, 3));

    auto dropdown = window->root_control()->find<ui::Dropdown>(TriggersWindow::Names::filter_dropdown);
    ASSERT_NE(dropdown, nullptr);

    auto dropdown_button = dropdown->find<ui::Button>(ui::Dropdown::Names::dropdown_button);
    ASSERT_NE(dropdown_button, nullptr);
    dropdown_button->clicked(Point());

    auto dropdown_list = dropdown->dropdown_listbox();
    ASSERT_NE(dropdown_list, nullptr);

    auto row = dropdown_list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Name");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_THAT(get_numbers(), ElementsAre(0, 1, 2));
}
