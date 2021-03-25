#include "TriggersWindow.h"
#include <trview.common/Colour.h>
#include <trview.app/Resources/resource.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Button.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Dropdown.h>
#include <trview.ui/Label.h>
#include <trview.common/Strings.h>
#include <trview.common/Windows/Clipboard.h>

namespace trview
{
    namespace
    {
        /// Colours commonly used in this class.
        namespace Colours
        {
            const Colour Divider{ 1.0f, 0.0f, 0.0f, 0.0f };
            const Colour LeftPanel{ 1.0f, 0.25f, 0.25f, 0.25f };
            const Colour ItemDetails{ 1.0f, 0.225f, 0.225f, 0.225f };
            const Colour Triggers{ 1.0f, 0.20f, 0.20f, 0.20f };
            const Colour DetailsBorder{ 0.0f, 0.0f, 0.0f, 0.0f };
        }

        ui::Listbox::Item create_listbox_item(const Trigger& item)
        {
            return { {{ L"#", std::to_wstring(item.number()) },
                     { L"Room", std::to_wstring(item.room()) },
                     { L"Type", trigger_type_name(item.type()) },
                     { L"Hide", std::to_wstring(!item.visible()) }}};
        }

        ui::Listbox::Item create_listbox_item_pointer(const Trigger* const item)
        {
            return create_listbox_item(*item);
        }
    }

    const std::string TriggersWindow::Names::add_to_route_button{ "AddToRoute" };
    const std::string TriggersWindow::Names::filter_dropdown{ "FilterDropdown" };
    const std::string TriggersWindow::Names::stats_listbox{ "StatsListbox" };
    const std::string TriggersWindow::Names::sync_trigger_checkbox{ "SyncTriggers" };
    const std::string TriggersWindow::Names::track_room_checkbox{ "TrackRoom" };
    const std::string TriggersWindow::Names::triggers_listbox{ "Triggers" };
    const std::string TriggersWindow::Names::trigger_commands_listbox{ "TriggerCommands" };

    using namespace graphics;

    TriggersWindow::TriggersWindow(const std::shared_ptr<graphics::IDevice>& device, const ui::render::IRenderer::RendererSource& renderer_source, const Window& parent)
        : CollapsiblePanel(device, renderer_source(Size(520, 400)), parent, L"trview.triggers", L"Triggers", Size(520, 400))
    {
        CollapsiblePanel::on_window_closed += ITriggersWindow::on_window_closed;
        set_panels(create_left_panel(), create_right_panel());
    }

    std::unique_ptr<ui::Control> TriggersWindow::create_left_panel()
    {
        using namespace ui;

        auto left_panel = std::make_unique<ui::StackPanel>(Size(250, window().size().height), Colours::LeftPanel, Size(0, 3), StackPanel::Direction::Vertical, SizeMode::Manual);
        left_panel->set_margin(Size(0, 3));

        // Control modes:.
        auto controls_box = left_panel->add_child(std::make_unique<StackPanel>(Size(250, 50), Colours::LeftPanel, Size(2, 2), StackPanel::Direction::Vertical, SizeMode::Manual));
        controls_box->set_margin(Size(2, 2));
        _controls = controls_box->add_child(std::make_unique<StackPanel>(Size(250, 20), Colours::LeftPanel, Size(2, 2), StackPanel::Direction::Horizontal, SizeMode::Manual));
        _controls->set_margin(Size(2, 2));
        _track_room_checkbox = _controls->add_child(std::make_unique<Checkbox>(Colours::LeftPanel, L"Track Room"));
        _track_room_checkbox->set_name(Names::track_room_checkbox);
        _token_store += _track_room_checkbox->on_state_changed += [this](bool value) { set_track_room(value); };

        // Spacing between checkboxes.
        _controls->add_child(std::make_unique<ui::Window>(Size(5, 20), Colours::LeftPanel));

        auto sync_trigger = _controls->add_child(std::make_unique<Checkbox>(Colours::LeftPanel, L"Sync Trigger"));
        sync_trigger->set_name(Names::sync_trigger_checkbox);
        sync_trigger->set_state(_sync_trigger);
        _token_store += sync_trigger->on_state_changed += [this](bool value) { set_sync_trigger(value); };

        // Space out the button
        _controls->add_child(std::make_unique<ui::Window>(Size(5, 20), Colours::LeftPanel));

        // Add the expander button at this point.
        add_expander(*_controls);

        // Command filter:
        auto controls_row2 = controls_box->add_child(std::make_unique<StackPanel>(Size(250, 20), Colours::LeftPanel, Size(2, 0), StackPanel::Direction::Horizontal, SizeMode::Manual));
        controls_row2->set_margin(Size(2, 0));

        _command_filter = controls_row2->add_child(std::make_unique<Dropdown>(Size(236, 20)));
        _command_filter->set_name(Names::filter_dropdown);
        std::vector<std::wstring> default_commands { L"All", L"Flipmaps" };
        _command_filter->set_values(default_commands);
        _command_filter->set_dropdown_scope(_ui.get());
        _command_filter->set_selected_value(L"All");
        _token_store += _command_filter->on_value_selected += [&](const auto& value) 
        {
            _selected_commands.clear();
            if (value == L"Flipmaps")
            {
                _selected_commands.push_back(TriggerCommandType::FlipMap);
                _selected_commands.push_back(TriggerCommandType::FlipOff);
                _selected_commands.push_back(TriggerCommandType::FlipOn);
            }
            else if (value != L"All")
            {
                _selected_commands.push_back(command_from_name(value));
            }
            apply_filters();
        };

        auto controls_box_bottom = controls_box->size().height;

        _triggers_list = left_panel->add_child(std::make_unique<Listbox>(Size(250, window().size().height - controls_box_bottom), Colours::LeftPanel));
        _triggers_list->set_name(Names::triggers_listbox);
        _triggers_list->set_columns(
            {
                { Listbox::Column::IdentityMode::Key, Listbox::Column::Type::Number, L"#", 30 },
                { Listbox::Column::IdentityMode::None, Listbox::Column::Type::Number, L"Room", 30 },
                { Listbox::Column::IdentityMode::None, Listbox::Column::Type::String, L"Type", 130 },
                { Listbox::Column::IdentityMode::None, Listbox::Column::Type::Boolean, L"Hide", 50 }
            }
        );
        _token_store += _triggers_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            load_trigger_details(*_all_triggers[index]);
            if (_sync_trigger)
            {
                on_trigger_selected(_all_triggers[index]);
            }
        };
        _token_store += _triggers_list->on_state_changed += [&](const auto item, const std::wstring& column, bool state)
        {
            if (column == L"Hide")
            {
                auto index = std::stoi(item.value(L"#"));
                on_trigger_visibility(_all_triggers[index], !state);
            }
        };

        // Fix items list size now that it has been added to the panel.
        _triggers_list->set_size(Size(250, left_panel->size().height - _triggers_list->position().y));

        return left_panel;
    }

    std::unique_ptr<ui::Control> TriggersWindow::create_right_panel()
    {
        using namespace ui;
        const float panel_width = 270;
        auto right_panel = std::make_unique<StackPanel>(Size(panel_width, window().size().height), Colours::ItemDetails, Size(), StackPanel::Direction::Vertical, SizeMode::Manual);
        right_panel->add_child(std::make_unique<ui::Window>(Size(panel_width, 8), Colours::ItemDetails));
        auto group_box = right_panel->add_child(std::make_unique<GroupBox>(Size(panel_width, 190), Colours::ItemDetails, Colours::DetailsBorder, L"Trigger Details"));
        auto details_panel = group_box->add_child(std::make_unique<StackPanel>(Size(panel_width - 20, 160), Colours::ItemDetails, Size(0, 16), StackPanel::Direction::Vertical, SizeMode::Manual));

        // Add some information about the selected item.
        _stats_list = details_panel->add_child(std::make_unique<Listbox>(Size(panel_width - 20, 120), Colours::ItemDetails));
        _stats_list->set_name(Names::stats_listbox);
        _stats_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"Name", 100 },
                { Listbox::Column::Type::Number, L"Value", 150 },
            }
        );
        _stats_list->set_show_headers(false);
        _stats_list->set_show_scrollbar(false);
        _stats_list->set_show_highlight(false);

        _token_store += _stats_list->on_item_selected += [this](const ui::Listbox::Item& item)
        {
            write_clipboard(window(), item.value(L"Value"));
        };

        auto button = details_panel->add_child(std::make_unique<Button>(Size(panel_width - 20, 20), L"Add to Route"));
        button->set_name(Names::add_to_route_button);
        _token_store += button->on_click += [&]()
        {
            if (_selected_trigger.has_value())
            {
                on_add_to_route(_selected_trigger.value());
            }
        };

        // Spacer element.
        right_panel->add_child(std::make_unique<ui::Window>(Size(panel_width, 5), Colours::Triggers));

        // Add the trigger details group box.
        auto command_group_box = right_panel->add_child(std::make_unique<GroupBox>(Size(panel_width, 200), Colours::Triggers, Colours::DetailsBorder, L"Commands"));
        _command_list = command_group_box->add_child(std::make_unique<Listbox>(Size(panel_width - 20, 160), Colours::Triggers));
        _command_list->set_name(Names::trigger_commands_listbox);
        _command_list->set_columns(
            {
                { Listbox::Column::Type::String, L"Type", 80 },
                { Listbox::Column::Type::String, L"Index", 35 },
                { Listbox::Column::Type::String, L"Entity", 125 },
            }
        );
        _command_list->set_show_headers(true);
        _command_list->set_show_scrollbar(true);
        _command_list->set_show_highlight(false);

        _token_store += _command_list->on_item_selected += [&](const auto& trigger_item)
        {
            auto index = std::stoi(trigger_item.value(L"#"));
            auto command = _selected_trigger.value()->commands()[index];
            if (command.type() == TriggerCommandType::LookAtItem || command.type() == TriggerCommandType::Object && command.index() < _all_items.size())
            {
                set_track_room(false);
                on_item_selected(_all_items[command.index()]);
            }
        };

        return right_panel;
    }

    void TriggersWindow::set_triggers(const std::vector<Trigger*>& triggers)
    {
        _all_triggers = triggers;
        populate_triggers(triggers);

        // Populate command filter dropdown.
        _selected_commands.clear();
        std::set<TriggerCommandType> command_set;
        for (const auto& trigger : triggers)
        {
            for (const auto& command : trigger->commands())
            {
                command_set.insert(command.type());
            }
        }
        std::vector<std::wstring> all_commands{ L"All", L"Flipmaps" };
        std::transform(command_set.begin(), command_set.end(), std::back_inserter(all_commands), command_type_name);
        _command_filter->set_values(all_commands);
        _command_filter->set_selected_value(L"All");
    }

    void TriggersWindow::update_triggers(const std::vector<Trigger*>& triggers)
    {
        _all_triggers = triggers;
        populate_triggers(triggers);
        apply_filters();
    }

    void TriggersWindow::clear_selected_trigger()
    {
        _selected_trigger.reset();
        _stats_list->set_items({});
        _command_list->set_items({});
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
            _current_room = room;
            _filter_applied = true;
            apply_filters();
        }

        _current_room = room;
    }

    void TriggersWindow::set_selected_trigger(const Trigger* const trigger)
    {
        _selected_trigger = trigger;
        if (_sync_trigger)
        {
            if (!_selected_commands.empty() && !trigger->has_any_command(_selected_commands))
            {
                _selected_commands.clear();
                _command_filter->set_selected_value(L"All");
                apply_filters();
            }

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
                _filter_applied = false;
                set_triggers(_all_triggers);
            }
        }

        if (_track_room_checkbox->state() != _track_room)
        {
            _track_room_checkbox->set_state(_track_room);
        }
    }

    void TriggersWindow::load_trigger_details(const Trigger& trigger)
    {
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

    void TriggersWindow::apply_filters()
    {
        auto room_filter = [&](const auto& trigger)
        {
            return !_filter_applied || trigger->room() == _current_room;
        };

        auto command_filter = [&](const auto& trigger)
        {
            return _selected_commands.empty() || trigger->has_any_command(_selected_commands);
        };

        std::vector<Trigger*> filtered_triggers;
        for (const auto& trigger : _all_triggers)
        {
            if (room_filter(trigger) && command_filter(trigger))
            {
                filtered_triggers.push_back(trigger);
            }
        }
        populate_triggers(filtered_triggers);
    }

    std::optional<const Trigger*> TriggersWindow::selected_trigger() const
    {
        return _selected_trigger;
    }

    void TriggersWindow::render(const graphics::IDevice& device, bool vsync)
    {
        CollapsiblePanel::render(device, vsync);
    }
}
