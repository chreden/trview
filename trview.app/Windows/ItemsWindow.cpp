#include "ItemsWindow.h"
#include <trview/resource.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Button.h>
#include <trview.common/Strings.h>
#include <trview.common/Windows/Clipboard.h>

using namespace trview::graphics;

namespace trview
{
    namespace
    {
        /// Colours commonly used in this class.
        namespace Colours
        {
            const Colour LeftPanel { 1.0f, 0.25f, 0.25f, 0.25f };
            const Colour ItemDetails { 1.0f, 0.225f, 0.225f, 0.225f };
            const Colour Triggers { 1.0f, 0.20f, 0.20f, 0.20f };
            const Colour DetailsBorder { 0.0f, 0.0f, 0.0f, 0.0f };
        }

        const float Height{ 420 };

        ui::Listbox::Item create_listbox_item(const Item& item)
        {
            return { {{ L"#", std::to_wstring(item.number()) },
                     { L"ID", std::to_wstring(item.type_id()) },
                     { L"Room", std::to_wstring(item.room()) },
                     { L"Type", item.type() },
                     { L"Hide", std::to_wstring(!item.visible()) }} };
        }
    }

    ItemsWindow::ItemsWindow(Device& device, const IShaderStorage& shader_storage, const FontFactory& font_factory, const Window& parent)
        : CollapsiblePanel(device, shader_storage, font_factory, parent, L"trview.items", L"Items", Size(450, Height))
    {
        set_panels(create_left_panel(), create_right_panel());
    }

    void ItemsWindow::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
        populate_items(items);
    }

    void ItemsWindow::update_items(const std::vector<Item>& items)
    {
        set_items(items);
        if (_track_room)
        {
            _filter_applied = false;
            set_current_room(_current_room);
        }
    }

    void ItemsWindow::set_triggers(const std::vector<Trigger*>& triggers)
    {
        _all_triggers = triggers;
        _trigger_list->set_items({});
    }

    void ItemsWindow::clear_selected_item()
    {
        _selected_item.reset();
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
        auto left_panel = std::make_unique<StackPanel>(Size(250, window().size().height), Colours::LeftPanel, Size(0, 3), StackPanel::Direction::Vertical, SizeMode::Manual);
        left_panel->set_margin(Size(0, 3));

        // Control modes:.
        _controls = left_panel->add_child(std::make_unique<StackPanel>(Size(200, 20), Colours::LeftPanel, Size(2, 2), StackPanel::Direction::Horizontal, SizeMode::Manual));
        _controls->set_margin(Size(2, 2));
        _track_room_checkbox = _controls->add_child(std::make_unique<Checkbox>(Colours::LeftPanel, L"Track Room"));
        _token_store += _track_room_checkbox->on_state_changed += [this](bool value)
        {
            set_track_room(value);
        };

        // Spacing between checkboxes.
        _controls->add_child(std::make_unique<ui::Window>(Size(10, 20), Colours::LeftPanel));

        auto sync_item = _controls->add_child(std::make_unique<Checkbox>(Colours::LeftPanel, L"Sync Item"));
        sync_item->set_state(_sync_item);
        _token_store += sync_item->on_state_changed += [this](bool value) { set_sync_item(value); };

        // Space out the button
        _controls->add_child(std::make_unique<ui::Window>(Size(15, 20), Colours::LeftPanel));

        // Add the expander button at this point.
        add_expander(*_controls);

        _items_list = left_panel->add_child(std::make_unique<Listbox>(Size(250, window().size().height - _controls->size().height), Colours::LeftPanel));
        _items_list->set_columns(
            {
                { Listbox::Column::IdentityMode::Key, Listbox::Column::Type::Number, L"#", 30 },
                { Listbox::Column::IdentityMode::None, Listbox::Column::Type::Number, L"Room", 30 },
                { Listbox::Column::IdentityMode::None, Listbox::Column::Type::Number, L"ID", 30 },
                { Listbox::Column::IdentityMode::None, Listbox::Column::Type::String, L"Type", 100 },
                { Listbox::Column::IdentityMode::None, Listbox::Column::Type::Boolean, L"Hide", 50 }
            }
        );
        _token_store += _items_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            load_item_details(_all_items[index]);
            if (_sync_item)
            {
                on_item_selected(_all_items[index]);
            }
        };
        _token_store += _items_list->on_state_changed += [&](const auto& item, const std::wstring& column, bool state)
        {
            if (column == L"Hide")
            {
                auto index = std::stoi(item.value(L"#"));
                on_item_visibility(_all_items[index], !state);
            }
        };

        // Fix items list size now that it has been added to the panel.
        _items_list->set_size(Size(250, left_panel->size().height - _items_list->position().y));

        return left_panel;
    }

    std::unique_ptr<ui::Control> ItemsWindow::create_right_panel()
    {
        using namespace ui;

        auto right_panel = std::make_unique<StackPanel>(Size(200, Height), Colours::ItemDetails, Size(), StackPanel::Direction::Vertical, SizeMode::Manual);
        right_panel->add_child(std::make_unique<ui::Window>(Size(200, 8), Colours::ItemDetails));

        auto group_box = right_panel->add_child(std::make_unique<GroupBox>(Size(200, 240), Colours::ItemDetails, Colours::DetailsBorder, L"Item Details"));
        auto details_panel = group_box->add_child(std::make_unique<StackPanel>(Size(180, 230), Colours::ItemDetails, Size(0, 8), StackPanel::Direction::Vertical, SizeMode::Manual));

        // Add some information about the selected item.
        _stats_list = details_panel->add_child(std::make_unique<Listbox>(Size(180, 180), Colours::ItemDetails));
        _stats_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"Name", 60 },
                { Listbox::Column::Type::Number, L"Value", 120 },
            }
        );
        _stats_list->set_show_headers(false);
        _stats_list->set_show_scrollbar(false);
        _stats_list->set_show_highlight(false);

        _token_store += _stats_list->on_item_selected += [this](const ui::Listbox::Item& item)
        {
            write_clipboard(window(), item.value(L"Value"));
        };

        auto add_to_route = details_panel->add_child(std::make_unique<Button>(Size(180, 20), L"Add to Route"));
        _token_store += add_to_route->on_click += [&]()
        {
            if (_selected_item.has_value())
            {
                on_add_to_route(_selected_item.value());
            }
        };

        // Spacer element.
        right_panel->add_child(std::make_unique<ui::Window>(Size(200, 5), Colours::Triggers));

        // Add the trigger details group box.
        auto trigger_group_box = right_panel->add_child(std::make_unique<GroupBox>(Size(200, 170), Colours::Triggers, Colours::DetailsBorder, L"Triggered By"));

        _trigger_list = trigger_group_box->add_child(std::make_unique<Listbox>(Size(190, 130), Colours::Triggers));
        _trigger_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 25 },
                { Listbox::Column::Type::Number, L"Room", 35 },
                { Listbox::Column::Type::String, L"Type", 120 },
            }
        );
        _trigger_list ->set_show_headers(true);
        _trigger_list ->set_show_scrollbar(true);
        _trigger_list ->set_show_highlight(true);

        _token_store += _trigger_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            set_track_room(false);
            on_trigger_selected(_all_triggers[index]);
        };

        return right_panel;
    }

    void ItemsWindow::load_item_details(const Item& item)
    {
        auto make_item = [](const auto& name, const auto& value)
        {
            return Listbox::Item { { { L"Name", name }, { L"Value", value } } };
        };

        auto position_text = [&item]()
        {
            std::wstringstream stream;
            stream.precision(0);
            stream << item.position().x * trlevel::Scale_X << L", " <<
                item.position().y * trlevel::Scale_Y << L", " <<
                item.position().z * trlevel::Scale_Z;
            return stream.str();
        };

        using namespace ui;
        std::vector<Listbox::Item> stats;
        stats.push_back(make_item(L"Type", item.type()));
        stats.push_back(make_item(L"#", std::to_wstring(item.number())));
        stats.push_back(make_item(L"Position", position_text()));
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
                        { L"#", std::to_wstring(trigger->number()) },
                        { L"Room", std::to_wstring(trigger->room()) },
                        { L"Type", trigger_type_name(trigger->type()) },
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
                _filter_applied = false;
                set_items(_all_items);
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
