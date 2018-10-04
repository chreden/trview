#include "ItemsWindow.h"
#include <sstream>
#include <bitset>
#include <Windows.h>
#include <trview/resource.h>
#include <SimpleMath.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Button.h>

using namespace trview::graphics;

namespace trview
{
    namespace
    {
        /// Colours commonly used in this class.
        namespace Colours
        {
            const Colour LeftPanel { 1.0f, 0.4f, 0.4f, 0.4f };
            const Colour ItemDetails { 1.0f, 0.35f, 0.35f, 0.35f };
            const Colour Triggers { 1.0f, 0.3f, 0.3f, 0.3f };
            const Colour DetailsBorder { 0.0f, 0.0f, 0.0f, 0.0f };
        }

        ui::Listbox::Item create_listbox_item(const Item& item)
        {
            return {{{ L"#", std::to_wstring(item.number()) },
                     { L"ID", std::to_wstring(item.type_id()) },
                     { L"Room", std::to_wstring(item.room()) },
                     { L"Type", item.type() }}};
        }
    }

    ItemsWindow::ItemsWindow(const Device& device, const IShaderStorage& shader_storage, const FontFactory& font_factory, HWND parent)
        : CollapsiblePanel(device, shader_storage, font_factory, parent, L"trview.items", L"Items")
    {
        set_panels(create_left_panel(), create_right_panel());
    }

    void ItemsWindow::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
        populate_items(items);
    }

    void ItemsWindow::set_triggers(const std::vector<Trigger>& triggers)
    {
        _all_triggers = triggers;
        _trigger_list->set_items({});
    }

    void ItemsWindow::clear_selected_item()
    {
        _stats_list->set_items({});
    }

    void ItemsWindow::populate_items(const std::vector<Item>& items)
    {
        using namespace ui;
        std::vector<Listbox::Item> list_items;
        std::transform(items.begin(), items.end(), std::back_inserter(list_items), create_listbox_item);
        _items_list->set_items(list_items);
    }

    void ItemsWindow::set_current_room(uint32_t room)
    {
        if (_track_room && (!_filter_applied || _current_room != room))
        {
            _filter_applied = true;

            std::vector<Item> filtered_items;
            for (const auto& item : _all_items)
            {
                if (item.room() == room)
                {
                    filtered_items.push_back(item);
                }
            }
            populate_items(filtered_items);
        }

        _current_room = room;
    }

    void ItemsWindow::update_layout()
    {
        CollapsiblePanel::update_layout();
        _items_list->set_size(Size(_items_list->size().width, _left_panel->size().height - _items_list->position().y));
    }

    std::unique_ptr<ui::Control> ItemsWindow::create_left_panel()
    {
        using namespace ui;
        auto left_panel = std::make_unique<ui::StackPanel>(Point(), Size(200, window().size().height), Colours::LeftPanel, Size(0, 3), StackPanel::Direction::Vertical, SizeMode::Manual);

        // Control modes:.
        auto controls = std::make_unique<StackPanel>(Point(), Size(200, 20), Colours::LeftPanel, Size(2, 2), StackPanel::Direction::Horizontal, SizeMode::Manual);
        auto track_room = std::make_unique<Checkbox>(Point(), Size(16, 16), Colours::LeftPanel, L"Track Room");
        _token_store.add(track_room->on_state_changed += [this](bool value)
        {
            set_track_room(value);
        });

        _track_room_checkbox = controls->add_child(std::move(track_room));

        // Spacing between checkboxes.
        controls->add_child(std::make_unique<ui::Window>(Point(), Size(10, 20), Colours::LeftPanel));

        auto sync_item = std::make_unique<Checkbox>(Point(), Size(16, 16), Colours::LeftPanel, L"Sync Item");
        sync_item->set_state(_sync_item);
        _token_store.add(sync_item->on_state_changed += [this](bool value) { set_sync_item(value); });
        controls->add_child(std::move(sync_item));

        // Space out the button
        controls->add_child(std::make_unique<ui::Window>(Point(), Size(15, 20), Colours::LeftPanel));

        // Add the expander button at this point.
        add_expander(*controls);

        _controls = left_panel->add_child(std::move(controls));

        auto items_list = std::make_unique<Listbox>(Point(), Size(200, window().size().height - _controls->size().height), Colours::LeftPanel);
        items_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 30 },
                { Listbox::Column::Type::Number, L"Room", 30 },
                { Listbox::Column::Type::Number, L"ID", 30 },
                { Listbox::Column::Type::String, L"Type", 100 } 
            }
        );
        _token_store.add(items_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            load_item_details(_all_items[index]);
            if (_sync_item)
            {
                on_item_selected(_all_items[index]);
            }
        });

        _items_list = items_list.get();
        left_panel->add_child(std::move(items_list));

        // Fix items list size now that it has been added to the panel.
        _items_list->set_size(Size(200, left_panel->size().height - _items_list->position().y));

        return left_panel;
    }

    std::unique_ptr<ui::Control> ItemsWindow::create_right_panel()
    {
        using namespace ui;

        const float height = 400;

        auto right_panel = std::make_unique<StackPanel>(Point(), Size(200, height), Colours::ItemDetails, Size(), StackPanel::Direction::Vertical, SizeMode::Manual);
        auto group_box = std::make_unique<GroupBox>(Point(), Size(200, 190), Colours::ItemDetails, Colours::DetailsBorder, L"Item Details");

        // Add some information about the selected item.
        auto stats_list = std::make_unique<Listbox>(Point(10,21), Size(180, 160), Colours::ItemDetails);
        stats_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"Name", 60 },
                { Listbox::Column::Type::Number, L"Value", 120 },
            }
        );
        stats_list->set_show_headers(false);
        stats_list->set_show_scrollbar(false);
        stats_list->set_show_highlight(false);

        _stats_list = group_box->add_child(std::move(stats_list));

        right_panel->add_child(std::make_unique<ui::Window>(Point(), Size(200, 8), Colours::ItemDetails));
        right_panel->add_child(std::move(group_box));

        // Spacer element.
        right_panel->add_child(std::make_unique<ui::Window>(Point(), Size(200, 5), Colours::Triggers));

        // Add the trigger details group box.
        auto trigger_group_box = std::make_unique<GroupBox>(Point(), Size(200, 200), Colours::Triggers, Colours::DetailsBorder, L"Triggered By");

        auto trigger_list = std::make_unique<Listbox>(Point(10, 21), Size(190, 160), Colours::Triggers);
        trigger_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 25 },
                { Listbox::Column::Type::Number, L"Room", 35 },
                { Listbox::Column::Type::String, L"Type", 120 },
            }
        );
        trigger_list->set_show_headers(true);
        trigger_list->set_show_scrollbar(true);
        trigger_list->set_show_highlight(false);

        _token_store.add(trigger_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            set_track_room(false);
            on_trigger_selected(_all_triggers[index]);
        });

        _trigger_list = trigger_group_box->add_child(std::move(trigger_list));
        right_panel->add_child(std::move(trigger_group_box));

        return right_panel;
    }

    void ItemsWindow::load_item_details(const Item& item)
    {
        auto format_bool = [](bool value)
        {
            std::wstringstream stream;
            stream << std::boolalpha << value;
            return stream.str();
        };

        auto format_binary = [](uint16_t value)
        {
            std::wstringstream stream;
            stream << std::bitset<5>(value);
            const auto result = stream.str();
            return std::wstring(result.rbegin(), result.rend());
        };

        auto make_item = [](const auto& name, const auto& value)
        {
            return Listbox::Item { { { L"Name", name }, { L"Value", value } } };
        };

        using namespace ui;
        std::vector<Listbox::Item> stats;
        stats.push_back(make_item(L"Type", item.type()));
        stats.push_back(make_item(L"#", std::to_wstring(item.number())));
        stats.push_back(make_item(L"Type ID", std::to_wstring(item.type_id())));
        stats.push_back(make_item(L"Room", std::to_wstring(item.room())));
        stats.push_back(make_item(L"Clear Body", format_bool(item.clear_body_flag())));
        stats.push_back(make_item(L"Invisible", format_bool(item.invisible_flag())));
        stats.push_back(make_item(L"Flags", format_binary(item.activation_flags())));
        stats.push_back(make_item(L"OCB", std::to_wstring(item.ocb())));
        _stats_list->set_items(stats);

        std::vector<Listbox::Item> triggers;
        for (auto& trigger : item.triggers())
        {
            triggers.push_back(
                {
                    {
                        { L"#", std::to_wstring(trigger.number()) },
                        { L"Room", std::to_wstring(trigger.room()) },
                        { L"Type", trigger_type_name(trigger.type()) },
                    }
                });
        }
        _trigger_list->set_items(triggers);
    }

    void ItemsWindow::set_track_room(bool value)
    {
        if (_track_room != value)
        {
            _track_room = value;
            if (_track_room)
            {
                set_current_room(_current_room);
            }
            else
            {
                set_items(_all_items);
                _filter_applied = false;
            }
        }

        if (_track_room_checkbox->state() != _track_room)
        {
            _track_room_checkbox->set_state(_track_room);
        }
    }

    void ItemsWindow::set_sync_item(bool value)
    {
        if (_sync_item != value)
        {
            _sync_item = value;
            if (_selected_item.has_value())
            {
                set_selected_item(_selected_item.value());
            }
        }
    }

    void ItemsWindow::set_selected_item(const Item& item)
    {
        _selected_item = item;
        if (_sync_item)
        {
            const auto& list_item = create_listbox_item(item);
            if (_items_list->set_selected_item(list_item))
            {
                load_item_details(item);
            }
            else
            {
                _selected_item.reset();
            }
        }
    }
}
