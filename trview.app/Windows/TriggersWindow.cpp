#include "TriggersWindow.h"
#include <trview.app/Resources/resource.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Button.h>
#include <trview.ui/Dropdown.h>
#include <trview.ui/Label.h>
#include <trview.common/Strings.h>
#include <trview.common/Windows/Clipboard.h>
#include <external/imgui/imgui.h>
#include <external/imgui/imgui_internal.h>

// TODO
// Most things
// Command filter

namespace trview
{
    namespace
    {
        ui::Listbox::Item create_listbox_item(const ITrigger& item)
        {
            return { {{ L"#", std::to_wstring(item.number()) },
                     { L"Room", std::to_wstring(item.room()) },
                     { L"Type", trigger_type_name(item.type()) },
                     { L"Hide", std::to_wstring(!item.visible()) }}};
        }

        ui::Listbox::Item create_listbox_item_pointer(const std::weak_ptr<ITrigger>& trigger)
        {
            if (const auto trigger_ptr = trigger.lock())
            {
                return create_listbox_item(*trigger_ptr);
            }
            return { {} };
        }
    }

    const std::string TriggersWindow::Names::add_to_route_button{ "AddToRoute" };
    const std::string TriggersWindow::Names::filter_dropdown{ "FilterDropdown" };
    const std::string TriggersWindow::Names::stats_listbox{ "StatsListbox" };
    const std::string TriggersWindow::Names::sync_trigger_checkbox{ "SyncTriggers" };
    const std::string TriggersWindow::Names::track_room_checkbox{ "TrackRoom" };
    const std::string TriggersWindow::Names::triggers_listbox{ "Triggers" };
    const std::string TriggersWindow::Names::trigger_commands_listbox{ "TriggerCommands" };
    const std::string TriggersWindow::Names::expander{ "Expander" };

    using namespace graphics;

    TriggersWindow::TriggersWindow(const IDeviceWindow::Source& device_window_source, const ui::render::IRenderer::Source& renderer_source,
        const ui::IInput::Source& input_source, const Window& parent, const std::shared_ptr<IClipboard>& clipboard, const IBubble::Source& bubble_source,
        const std::shared_ptr<ui::ILoader>& ui_source)
        : CollapsiblePanel(device_window_source, renderer_source(Size(520, 400)), parent, L"trview.triggers", L"Triggers", input_source, Size(520, 400), ui_source->load_from_resource(IDR_UI_TRIGGERS_WINDOW)),
        _clipboard(clipboard), _bubble(bubble_source(*_ui))
    {
        CollapsiblePanel::on_window_closed += ITriggersWindow::on_window_closed;
        bind_controls();
    }

    void TriggersWindow::bind_controls()
    {
        using namespace ui;
        _track_room_checkbox = _ui->find<Checkbox>(Names::track_room_checkbox);
        _token_store += _track_room_checkbox->on_state_changed += [this](bool value) { set_track_room(value); };

        auto sync_trigger = _ui->find<Checkbox>(Names::sync_trigger_checkbox);
        sync_trigger->set_state(_sync_trigger);
        _token_store += sync_trigger->on_state_changed += [this](bool value) { set_sync_trigger(value); };

        set_expander(_ui->find<Button>(Names::expander));

        _command_filter = _ui->find<Dropdown>(Names::filter_dropdown);
        _command_filter->set_dropdown_scope(_ui.get());
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

        _triggers_list = _ui->find<Listbox>(Names::triggers_listbox);
        _token_store += _triggers_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            if (auto trigger = _all_triggers[index].lock())
            {
                load_trigger_details(*trigger);
            }
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

        _stats_list = _ui->find<Listbox>(Names::stats_listbox);
        _token_store += _stats_list->on_item_selected += [this](const ui::Listbox::Item& item)
        {
            _clipboard->write(window(), item.value(L"Value"));
            _bubble->show(client_cursor_position(window()) - Point(0, 20));
        };

        auto button = _ui->find<Button>(Names::add_to_route_button);
        _token_store += button->on_click += [&]()
        {
            if (const auto trigger_ptr = _selected_trigger.lock())
            {
                on_add_to_route(_selected_trigger);
            }
        };

        _command_list = _ui->find<Listbox>(Names::trigger_commands_listbox);
        _token_store += _command_list->on_item_selected += [&](const auto& trigger_item)
        {
            auto index = std::stoi(trigger_item.value(L"#"));
            if (const auto trigger = _selected_trigger.lock())
            {
                auto command = trigger->commands()[index];
                if (command.type() == TriggerCommandType::LookAtItem || command.type() == TriggerCommandType::Object && command.index() < _all_items.size())
                {
                    set_track_room(false);
                    on_item_selected(_all_items[command.index()]);
                }
            }
        };
    }

    void TriggersWindow::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _all_triggers = triggers;
        populate_triggers(triggers);

        _command_list->set_items({});
        _stats_list->set_items({});

        // Populate command filter dropdown.
        _selected_commands.clear();
        std::set<TriggerCommandType> command_set;
        for (const auto& trigger : triggers)
        {
            const auto trigger_ptr = trigger.lock();
            for (const auto& command : trigger_ptr->commands())
            {
                command_set.insert(command.type());
            }
        }
        std::vector<std::wstring> all_commands{ L"All", L"Flipmaps" };
        std::transform(command_set.begin(), command_set.end(), std::back_inserter(all_commands), command_type_name);
        _command_filter->set_values(all_commands);
        _command_filter->set_selected_value(L"All");
    }

    void TriggersWindow::update_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _all_triggers = triggers;
        populate_triggers(triggers);
        apply_filters();
    }

    void TriggersWindow::clear_selected_trigger()
    {
        _selected_trigger.reset();
        _triggers_list->clear_selection();
        _stats_list->set_items({});
        _command_list->set_items({});
    }

    void TriggersWindow::populate_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
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

    void TriggersWindow::set_selected_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        _selected_trigger = trigger;
        if (_sync_trigger)
        {
            const auto trigger_ptr = trigger.lock();
            if (!trigger_ptr)
            {
                return;
            }

            if (!_selected_commands.empty() && !has_any_command(*trigger_ptr, _selected_commands))
            {
                _selected_commands.clear();
                _command_filter->set_selected_value(L"All");
                apply_filters();
            }

            const auto& list_item = create_listbox_item(*trigger_ptr);
            if (_triggers_list->set_selected_item(list_item))
            {
                load_trigger_details(*trigger_ptr);
            }
            else
            {
                _selected_trigger.reset();
            }
        }
    }

    void TriggersWindow::set_sync_trigger(bool value)
    {
        if (_sync_trigger != value)
        {
            _sync_trigger = value;
            set_selected_trigger(_selected_trigger);
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

    void TriggersWindow::load_trigger_details(const ITrigger& trigger)
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
            return _selected_commands.empty() || has_any_command(*trigger, _selected_commands);
        };

        std::vector<std::weak_ptr<ITrigger>> filtered_triggers;
        for (const auto& trigger : _all_triggers)
        {
            const auto trigger_ptr = trigger.lock();
            if (room_filter(trigger_ptr) && command_filter(trigger_ptr))
            {
                filtered_triggers.push_back(trigger);
            }
        }
        populate_triggers(filtered_triggers);
    }

    std::weak_ptr<ITrigger> TriggersWindow::selected_trigger() const
    {
        return _selected_trigger;
    }

    void TriggersWindow::render(bool vsync)
    {
        CollapsiblePanel::render(vsync);

        if (!render_host())
        {
            ITriggersWindow::on_window_closed();
            return;
        }

        render_triggers_list();
        render_trigger_details();
    }

    void TriggersWindow::update(float delta)
    {
        _ui->update(delta);
    }

    bool TriggersWindow::render_host()
    {
        bool stay_open = true;
        ImGui::Begin("Triggers", &stay_open);
        ImGuiID dockspaceID = ImGui::GetID("dockspace");
        if (!ImGui::DockBuilderGetNode(dockspaceID))
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();

            ImGui::DockBuilderRemoveNode(dockspaceID);
            ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoTabBar);
            ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

            ImGuiID dock_main_id = dockspaceID;
            ImGui::DockBuilderDockWindow("TriggersList", dock_main_id);
            ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.2f, NULL, &dock_main_id);
            ImGui::DockBuilderDockWindow("TriggersDetails", dock_id_right);

            ImGui::DockBuilderGetNode(dock_main_id)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
            ImGui::DockBuilderGetNode(dock_id_right)->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

            ImGui::DockBuilderFinish(dockspaceID);
        }
        ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), 0);
        ImGui::End();
        return stay_open;
    }

    void TriggersWindow::render_triggers_list()
    {
        // TODO: Unique ID.
        if (ImGui::Begin("TriggersList", nullptr, ImGuiWindowFlags_NoTitleBar))
        {
            bool track_room = _track_room;
            if (ImGui::Checkbox("Track Room##trackroom", &track_room))
            {
                set_track_room(track_room);
            }
            ImGui::SameLine();
            bool sync_trigger = _sync_trigger;
            if (ImGui::Checkbox("Sync Trigger##synctrigger", &sync_trigger))
            {
                set_sync_trigger(sync_trigger);
            }

            ImGui::ShowStackToolWindow();

            if (ImGui::BeginTable("##triggerslist", 4, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Room");
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupColumn("Hide");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                auto specs = ImGui::TableGetSortSpecs();
                if (specs && specs->SpecsDirty)
                {
                    std::sort(_all_items.begin(), _all_items.end(),
                        [&](const auto& l, const auto& r) -> int
                        {
                            switch (specs->Specs[0].ColumnIndex)
                            {
                            case 0:
                                return specs->Specs->SortDirection == ImGuiSortDirection_Ascending
                                    ? (l.number() < r.number()) : (l.number() > r.number());
                            case 1:
                                return specs->Specs->SortDirection == ImGuiSortDirection_Ascending
                                    ? (l.room() < r.room()) : (l.room() > r.room());
                            case 2:
                                return specs->Specs->SortDirection == ImGuiSortDirection_Ascending
                                    ? (l.type() < r.type()) : (l.type() > r.type());
                            case 3:
                                return specs->Specs->SortDirection == ImGuiSortDirection_Ascending
                                    ? (l.visible() < r.visible()) : (l.visible() > r.visible());
                            }
                            return 0;
                        });
                    specs->SpecsDirty = false;
                }

                for (const auto& trigger : _all_triggers)
                {
                    const auto trigger_ptr = trigger.lock();
                    if (_track_room && trigger_ptr->room() != _current_room)
                    {
                        continue;
                    }

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    bool selected = _selected_trigger.lock() && _selected_trigger.lock()->number() == trigger_ptr->number();
                    if (ImGui::Selectable((std::to_string(trigger_ptr->number()) + std::string("##") + std::to_string(trigger_ptr->number())).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_SelectOnNav))
                    {
                        _selected_trigger = trigger;
                        if (_sync_trigger)
                        {
                            on_trigger_selected(trigger);
                        }
                    }
                    ImGui::SetItemAllowOverlap();
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(trigger_ptr->room()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(to_utf8(trigger_type_name(trigger_ptr->type())).c_str());
                    ImGui::TableNextColumn();
                    bool hidden = !trigger_ptr->visible();
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (ImGui::Checkbox((std::string("##hide-") + std::to_string(trigger_ptr->number())).c_str(), &hidden))
                    {
                        on_trigger_visibility(trigger, !hidden);
                    }
                    ImGui::PopStyleVar();
                }
                ImGui::EndTable();
            }
        }
        ImGui::End();
    }

    void TriggersWindow::render_trigger_details()
    {
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

        if (ImGui::Begin("TriggersDetails", nullptr, ImGuiWindowFlags_NoTitleBar))
        {
            ImGui::Text("Trigger Details");
            if (ImGui::BeginTable("##triggerstats", 2, 0, ImVec2(-1, 150)))
            {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Value");
                ImGui::TableNextRow();

                auto selected_trigger = _selected_trigger.lock();
                if (selected_trigger)
                {
                    auto add_stat = [&](const std::string& name, const std::string& value)
                    {
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                        {
                            _clipboard->write(window(), to_utf16(value));
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(value.c_str());
                    };

                    auto position_text = [&selected_trigger]()
                    {
                        std::stringstream stream;
                        stream << std::fixed << std::setprecision(0) <<
                            selected_trigger->position().x * trlevel::Scale_X << ", " <<
                            selected_trigger->position().y * trlevel::Scale_Y << ", " <<
                            selected_trigger->position().z * trlevel::Scale_Z;
                        return stream.str();
                    };

                    add_stat("Type", to_utf8(trigger_type_name(selected_trigger->type())));
                    add_stat("#", std::to_string(selected_trigger->number()));
                    add_stat("Position", position_text());
                    add_stat("Room", std::to_string(selected_trigger->room()));
                    add_stat("Flags", to_utf8(format_binary(selected_trigger->flags())));
                    add_stat("Only once", to_utf8(format_bool(selected_trigger->only_once())));
                    add_stat("Timer", std::to_string(selected_trigger->timer()));
                }

                ImGui::EndTable();
            }
            if (ImGui::Button("Add to Route##addtoroute", ImVec2(-1, 30)))
            {
                on_add_to_route(_selected_trigger);
            }
            ImGui::Text("Triggered By");
            if (ImGui::BeginTable("##triggeredby", 3, ImGuiTableFlags_ScrollY, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Room");
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                auto selected_trigger = _selected_trigger.lock();
                if (selected_trigger)
                {
                    for (auto& command : selected_trigger->commands())
                    {
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        bool selected = false;
                        if (ImGui::Selectable(std::to_string(command.number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_SelectOnNav))
                        {
                            if (command.type() == TriggerCommandType::LookAtItem || command.type() == TriggerCommandType::Object && command.index() < _all_items.size())
                            {
                                set_track_room(false);
                                on_item_selected(_all_items[command.index()]);
                            }
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(std::to_string(command.index()).c_str());
                        ImGui::TableNextColumn();
                        ImGui::Text(to_utf8(get_command_display(command)).c_str());;
                    }
                }

                ImGui::EndTable();
            }
        }
        ImGui::End();
    }
}
