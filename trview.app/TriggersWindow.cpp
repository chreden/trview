#include <sstream>
#include <bitset>

#include "TriggersWindow.h"
#include <trview.common/Colour.h>
#include <trview/resource.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Button.h>
#include <trview.ui/GroupBox.h>

namespace trview
{
    namespace
    {
        /// Colours commonly used in this class.
        namespace Colours
        {
            const Colour Divider{ 1.0f, 0.0f, 0.0f, 0.0f };
            const Colour LeftPanel{ 1.0f, 0.4f, 0.4f, 0.4f };
            const Colour ItemDetails{ 1.0f, 0.35f, 0.35f, 0.35f };
            const Colour Triggers{ 1.0f, 0.3f, 0.3f, 0.3f };
            const Colour DetailsBorder{ 0.0f, 0.0f, 0.0f, 0.0f };
        }

        ui::Listbox::Item create_listbox_item(const Trigger& item)
        {
            return { {{ L"#", std::to_wstring(item.number()) },
                     { L"Room", std::to_wstring(item.room()) },
                     { L"Type", trigger_type_name(item.type()) }} };
        }

        ui::Listbox::Item create_listbox_item_pointer(const Trigger* const item)
        {
            return create_listbox_item(*item);
        }
    }

    using namespace graphics;

    TriggersWindow::TriggersWindow(const Device& device, const IShaderStorage& shader_storage, const FontFactory& font_factory, HWND parent)
        : CollapsiblePanel(device, shader_storage, font_factory, parent, L"trview.triggers", L"Triggers", Size(470, 400))
    {
        set_panels(create_left_panel(), create_right_panel());
    }

    std::unique_ptr<ui::Control> TriggersWindow::create_left_panel()
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
        controls->add_child(std::make_unique<ui::Window>(Point(), Size(5, 20), Colours::LeftPanel));

        auto sync_trigger = std::make_unique<Checkbox>(Point(), Size(16, 16), Colours::LeftPanel, L"Sync Trigger");
        sync_trigger->set_state(_sync_trigger);
        _token_store.add(sync_trigger->on_state_changed += [this](bool value) { set_sync_trigger(value); });
        controls->add_child(std::move(sync_trigger));

        // Space out the button
        controls->add_child(std::make_unique<ui::Window>(Point(), Size(5, 20), Colours::LeftPanel));

        // Add the expander button at this point.
        add_expander(*controls);

        _controls = left_panel->add_child(std::move(controls));

        auto triggers_list = std::make_unique<Listbox>(Point(), Size(200, window().size().height - _controls->size().height), Colours::LeftPanel);
        triggers_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 30 },
                { Listbox::Column::Type::Number, L"Room", 30 },
                { Listbox::Column::Type::String, L"Type", 130 }
            }
        );
        _token_store.add(triggers_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            load_trigger_details(*_all_triggers[index]);
            if (_sync_trigger)
            {
                on_trigger_selected(_all_triggers[index]);
            }
        });

        _triggers_list = triggers_list.get();
        left_panel->add_child(std::move(triggers_list));

        // Fix items list size now that it has been added to the panel.
        _triggers_list->set_size(Size(200, left_panel->size().height - _triggers_list->position().y));

        return left_panel;
    }

    std::unique_ptr<ui::Control> TriggersWindow::create_right_panel()
    {
        using namespace ui;
        const float panel_width = 270;
        auto right_panel = std::make_unique<StackPanel>(Point(), Size(panel_width, window().size().height), Colours::ItemDetails, Size(), StackPanel::Direction::Vertical, SizeMode::Manual);
        auto group_box = std::make_unique<GroupBox>(Point(), Size(panel_width, 190), Colours::ItemDetails, Colours::DetailsBorder, L"Trigger Details");

        // Add some information about the selected item.
        auto stats_list = std::make_unique<Listbox>(Point(10, 21), Size(panel_width - 20, 160), Colours::ItemDetails);
        stats_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"Name", 100 },
                { Listbox::Column::Type::Number, L"Value", 150 },
            }
        );
        stats_list->set_show_headers(false);
        stats_list->set_show_scrollbar(false);
        stats_list->set_show_highlight(false);
        _stats_list = group_box->add_child(std::move(stats_list));

        right_panel->add_child(std::make_unique<ui::Window>(Point(), Size(panel_width, 8), Colours::ItemDetails));
        right_panel->add_child(std::move(group_box));

        // Spacer element.
        right_panel->add_child(std::make_unique<ui::Window>(Point(), Size(panel_width, 5), Colours::Triggers));

        // Add the trigger details group box.
        auto command_group_box = std::make_unique<GroupBox>(Point(), Size(panel_width, 200), Colours::Triggers, Colours::DetailsBorder, L"Commands");

        auto command_list = std::make_unique<Listbox>(Point(10, 21), Size(panel_width - 20, 160), Colours::Triggers);
        command_list->set_columns(
            {
                { Listbox::Column::Type::String, L"Type", 80 },
                { Listbox::Column::Type::String, L"Index", 35 },
                { Listbox::Column::Type::String, L"Entity", 125 },
            }
        );
        command_list->set_show_headers(true);
        command_list->set_show_scrollbar(true);
        command_list->set_show_highlight(false);

        _token_store.add(command_list->on_item_selected += [&](const auto& trigger_item)
        {
            auto index = std::stoi(trigger_item.value(L"#"));
            auto command = _selected_trigger.value()->commands()[index];
            if (command.type() == TriggerCommandType::LookAtItem || command.type() == TriggerCommandType::Object && command.index() < _all_items.size())
            {
                set_track_room(false);
                on_item_selected(_all_items[command.index()]);
            }
        });

        _command_list = command_group_box->add_child(std::move(command_list));
        right_panel->add_child(std::move(command_group_box));

        return right_panel;
    }

    void TriggersWindow::set_triggers(const std::vector<Trigger*>& triggers)
    {
        _all_triggers = triggers;
        populate_triggers(triggers);
    }

    void TriggersWindow::clear_selected_trigger()
    {
    }

    void TriggersWindow::populate_triggers(const std::vector<Trigger*>& triggers)
    {
        using namespace ui;
        std::vector<Listbox::Item> list_items;
        std::transform(triggers.begin(), triggers.end(), std::back_inserter(list_items), create_listbox_item_pointer);
        _triggers_list->set_items(list_items);
    }

    void TriggersWindow::set_current_room(uint32_t room)
    {
        if (_track_room && (!_filter_applied || _current_room != room))
        {
            _filter_applied = true;

            std::vector<Trigger*> filtered_triggers;
            for (const auto& trigger : _all_triggers)
            {
                if (trigger->room() == room)
                {
                    filtered_triggers.push_back(trigger);
                }
            }
            populate_triggers(filtered_triggers);
        }

        _current_room = room;
    }

    void TriggersWindow::set_selected_trigger(const Trigger* const trigger)
    {
        _selected_trigger = trigger;
        if (_sync_trigger)
        {
            const auto& list_item = create_listbox_item(*trigger);
            if (_triggers_list->set_selected_item(list_item))
            {
                load_trigger_details(*trigger);
            }
            else
            {
                _selected_trigger.reset();
            }
        }
    }

    void TriggersWindow::update_layout()
    {
        CollapsiblePanel::update_layout();
        _triggers_list->set_size(Size(_triggers_list->size().width, _left_panel->size().height - _triggers_list->position().y));
    }

    void TriggersWindow::set_sync_trigger(bool value)
    {
        if (_sync_trigger != value)
        {
            _sync_trigger = value;
            if (_selected_trigger.has_value())
            {
                set_selected_trigger(_selected_trigger.value());
            }
        }
    }

    void TriggersWindow::set_track_room(bool value)
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
                set_triggers(_all_triggers);
                _filter_applied = false;
            }
        }

        if (_track_room_checkbox->state() != _track_room)
        {
            _track_room_checkbox->set_state(_track_room);
        }
    }

    void TriggersWindow::load_trigger_details(const Trigger& trigger)
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
            return Listbox::Item{ { { L"Name", name }, { L"Value", value } } };
        };

        auto get_command_display = [this](const Command& command)
        {
            if (command.type() == TriggerCommandType::LookAtItem || command.type() == TriggerCommandType::Object)
            {
                if (command.index() < _all_items.size())
                {
                    return _all_items[command.index()].type();
                }
                return std::wstring(L"No Item");
            }
            return std::wstring();
        };

        using namespace ui;
        std::vector<Listbox::Item> stats;
        stats.push_back(make_item(L"Type", trigger_type_name(trigger.type())));
        stats.push_back(make_item(L"#", std::to_wstring(trigger.number())));
        stats.push_back(make_item(L"Room", std::to_wstring(trigger.room())));
        stats.push_back(make_item(L"Flags", format_binary(trigger.flags())));
        stats.push_back(make_item(L"Only once", format_bool(trigger.only_once())));
        stats.push_back(make_item(L"Timer", std::to_wstring(trigger.timer())));
        _stats_list->set_items(stats);

        std::vector<Listbox::Item> commands;
        for (auto& command : trigger.commands())
        {
            commands.push_back(
                {
                    {
                        { L"#", std::to_wstring(command.number()) },
                        { L"Type", command_type_name(command.type()) },
                        { L"Index", std::to_wstring(command.index()) },
                        { L"Entity", get_command_display(command) },
                    }
                });
        }
        _command_list->set_items(commands);
    }

    void TriggersWindow::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
    }
}
