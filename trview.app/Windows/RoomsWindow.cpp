#include "RoomsWindow.h"
#include <trview.ui/StackPanel.h>
#include <trview.ui/GroupBox.h>
#include <trview.app/Elements/Room.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/Trigger.h>
#include <numeric>

namespace trview
{
    namespace
    {
        namespace Colours
        {
            const Colour Divider{ 1.0f, 0.0f, 0.0f, 0.0f };
            const Colour LeftPanel{ 1.0f, 0.25f, 0.25f, 0.25f };
            const Colour ItemDetails{ 1.0f, 0.225f, 0.225f, 0.225f };
            const Colour Triggers{ 1.0f, 0.20f, 0.20f, 0.20f };
            const Colour DetailsBorder{ 0.0f, 0.0f, 0.0f, 0.0f };
        }

        ui::Listbox::Item create_listbox_item(const Room* const room, const std::vector<Item>& items, const std::vector<Trigger*>& triggers)
        {
            auto item_count = std::count_if(items.begin(), items.end(), [&room](const auto& item) { return item.room() == room->number(); });
            auto trigger_count = std::count_if(triggers.begin(), triggers.end(), [&room](const auto& trigger) { return trigger->room() == room->number(); });

            return
            {
                {
                    { L"#", std::to_wstring(room->number()) },
                    { L"X", std::to_wstring(room->info().x) },
                    { L"Z", std::to_wstring(room->info().z) },
                    { L"Items", std::to_wstring(item_count) },
                    { L"Triggers", std::to_wstring(trigger_count) }
                }
            };
        }

        ui::Listbox::Item create_listbox_item(const Item& item)
        {
            return { {{ L"#", std::to_wstring(item.number()) },
                     { L"ID", std::to_wstring(item.type_id()) },
                     { L"Room", std::to_wstring(item.room()) },
                     { L"Type", item.type() }} };
        }

        ui::Listbox::Item create_listbox_item(const Trigger& item)
        {
            return { {{ L"#", std::to_wstring(item.number()) },
                     { L"Type", trigger_type_name(item.type()) }} };
        }
    }

    RoomsWindow::RoomsWindow(graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::FontFactory& font_factory, const Window& parent)
        : CollapsiblePanel(device, shader_storage, font_factory, parent, L"trview.rooms", L"Rooms", Size(850, 800))
    {
        set_panels(create_left_panel(), create_right_panel());
    }

    std::unique_ptr<ui::Control> RoomsWindow::create_left_panel()
    {
        using namespace ui;
        auto left_panel = std::make_unique<StackPanel>(Size(250, window().size().height), Colours::LeftPanel, Size(0, 3), StackPanel::Direction::Vertical, SizeMode::Manual);

        auto controls = std::make_unique<StackPanel>(Size(250, 20), Colours::LeftPanel, Size(2, 2), StackPanel::Direction::Horizontal, SizeMode::Manual);
        controls->set_margin(Size(2, 2));
        auto track_room = std::make_unique<Checkbox>(Colours::LeftPanel, L"Track Room");
        track_room->set_state(true);
        _token_store += track_room->on_state_changed += [this](bool value)
        {
            set_track_room(value);
        };

        _track_room_checkbox = controls->add_child(std::move(track_room));

        _controls = left_panel->add_child(std::move(controls));
        _rooms_list = left_panel->add_child(create_rooms_list());

        // Fix items list size now that it has been added to the panel.
        _rooms_list->set_size(Size(250, left_panel->size().height - _rooms_list->position().y));

        return left_panel;
    }

    std::unique_ptr<ui::Listbox> RoomsWindow::create_rooms_list()
    {
        using namespace ui;

        auto rooms_list = std::make_unique<Listbox>(Size(250, window().size().height - _controls->size().height), Colours::LeftPanel);
        rooms_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 30 },
                { Listbox::Column::Type::Number, L"X", 50 },
                { Listbox::Column::Type::Number, L"Z", 50 },
                { Listbox::Column::Type::Number, L"Items", 50 },
                { Listbox::Column::Type::Number, L"Triggers", 50 }
            }
        );
        _token_store += rooms_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            load_room_details(*_all_rooms[index]);
            if (_track_room)
            {
                on_room_selected(index);
            }
        };
        return rooms_list;
    }

    void RoomsWindow::create_items_list(ui::Control& parent)
    {
        using namespace ui;

        auto group_box = std::make_unique<GroupBox>(Size(150, window().size().height), Colours::ItemDetails, Colours::DetailsBorder, L"Items");
        auto items_list = std::make_unique<Listbox>(Point(10, 21), Size(140, window().size().height - 21), Colours::LeftPanel);
        items_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 30 },
                { Listbox::Column::Type::Number, L"Type", 100 }
            }
        );
        _token_store += items_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            if (_sync_item)
            {
                on_item_selected(_all_items[index]);
            }
        };
        _items_list = group_box->add_child(std::move(items_list));
        parent.add_child(std::move(group_box));
    }

    void RoomsWindow::create_triggers_list(ui::Control& parent)
    {
        using namespace ui;

        auto group_box = std::make_unique<GroupBox>(Size(150, window().size().height), Colours::ItemDetails, Colours::DetailsBorder, L"Triggers");
        auto triggers_list = std::make_unique<Listbox>(Point(10, 21), Size(140, window().size().height - 21), Colours::LeftPanel);
        triggers_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 30 },
                { Listbox::Column::Type::Number, L"Type", 100 }
            }
        );
        _token_store += triggers_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            if (_sync_trigger)
            {
                on_trigger_selected(_all_triggers[index]);
            }
        };
        _triggers_list = group_box->add_child(std::move(triggers_list));
        parent.add_child(std::move(group_box));
    }

    void RoomsWindow::set_current_room(uint32_t room)
    {
        _current_room = room;
        if (_track_room && _current_room < _all_rooms.size())
        {
            auto item = create_listbox_item(_all_rooms[room], _all_items, _all_triggers);
            _rooms_list->set_selected_item(item);

            load_room_details(*_all_rooms[_current_room]);
        }
    }

    void RoomsWindow::set_items(const std::vector<Item>& items)
    {
        using namespace ui;
        _items_list->set_items({});
        _all_items = items;
    }

    void RoomsWindow::set_rooms(const std::vector<Room*>& rooms)
    {
        using namespace ui;
        std::vector<Listbox::Item> list_items;
        std::transform(rooms.begin(), rooms.end(), std::back_inserter(list_items), [&](const auto& room) { return create_listbox_item(room, _all_items, _all_triggers); });
        _rooms_list->set_items(list_items);
        _all_rooms = rooms;
    }

    void RoomsWindow::set_triggers(const std::vector<Trigger*>& triggers)
    {
        _triggers_list->set_items({});
        _all_triggers = triggers;
    }

    void RoomsWindow::load_room_details(const Room& room)
    {
        using namespace ui;

        std::vector<Listbox::Item> list_items;
        for (const auto& item : _all_items)
        {
            if (item.room() == room.number())
            {
                list_items.push_back(create_listbox_item(item));
            }
        }
        _items_list->set_items(list_items);

        std::vector<Listbox::Item> list_triggers;
        for (const auto& trigger : _all_triggers)
        {
            if (trigger->room() == room.number())
            {
                list_triggers.push_back(create_listbox_item(*trigger));
            }
        }
        _triggers_list->set_items(list_triggers);
    }

    std::unique_ptr<ui::Control> RoomsWindow::create_right_panel()
    {
        using namespace ui;
        const float panel_width = 600;
        auto right_panel = std::make_unique<StackPanel>(Size(panel_width, window().size().height), Colours::ItemDetails, Size(), StackPanel::Direction::Horizontal, SizeMode::Manual);
        right_panel->set_margin(Size(0, 2));
        create_items_list(*right_panel);
        create_triggers_list(*right_panel);

        return right_panel;
    }

    void RoomsWindow::set_track_room(bool value)
    {
        if (_track_room != value)
        {
            _track_room = value;
            set_current_room(_current_room);
        }

        if (_track_room_checkbox->state() != _track_room)
        {
            _track_room_checkbox->set_state(_track_room);
        }
    }
}