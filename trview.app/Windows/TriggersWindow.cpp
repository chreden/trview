#include "TriggersWindow.h"
#include <trview.common/Strings.h>
#include <trview.common/Windows/Clipboard.h>

// TODO
// Most things
// Command filter
// Fix sorting (commands sorted by trigger list)
// Fix sync trigger

namespace trview
{
    const std::string TriggersWindow::Names::add_to_route_button{ "AddToRoute" };
    const std::string TriggersWindow::Names::filter_dropdown{ "FilterDropdown" };
    const std::string TriggersWindow::Names::stats_listbox{ "StatsListbox" };
    const std::string TriggersWindow::Names::sync_trigger_checkbox{ "SyncTriggers" };
    const std::string TriggersWindow::Names::track_room_checkbox{ "TrackRoom" };
    const std::string TriggersWindow::Names::triggers_listbox{ "Triggers" };
    const std::string TriggersWindow::Names::trigger_commands_listbox{ "TriggerCommands" };
    const std::string TriggersWindow::Names::expander{ "Expander" };

    using namespace graphics;

    TriggersWindow::TriggersWindow(const Window& parent, const std::shared_ptr<IClipboard>& clipboard)
        : _clipboard(clipboard), _window(parent)
    {
        static int number = 0;
        _id = "Triggers " + std::to_string(++number);
    }

    void TriggersWindow::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _all_triggers = triggers;

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
        std::vector<std::string> all_commands{ "All", "Flipmaps" };
        std::transform(command_set.begin(), command_set.end(), std::back_inserter(all_commands), command_type_name_8);
        _all_commands = all_commands;
    }

    void TriggersWindow::update_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _all_triggers = triggers;
    }

    void TriggersWindow::clear_selected_trigger()
    {
        _selected_trigger.reset();
    }

    void TriggersWindow::set_current_room(uint32_t room)
    {
        _current_room = room;
    }

    void TriggersWindow::set_selected_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        _global_selected_trigger = trigger;
        if (_sync_trigger)
        {
            _scroll_to_trigger = true;
            _selected_trigger = trigger;
        }
    }

    void TriggersWindow::set_sync_trigger(bool value)
    {
        if (_sync_trigger != value)
        {
            _sync_trigger = value;
            _scroll_to_trigger = true;
            if (_sync_trigger && _global_selected_trigger.lock())
            {
                set_selected_trigger(_global_selected_trigger);
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
    }

    void TriggersWindow::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
    }

    std::weak_ptr<ITrigger> TriggersWindow::selected_trigger() const
    {
        return _selected_trigger;
    }

    void TriggersWindow::render(bool vsync)
    {
        if (!render_triggers_window())
        {
            on_window_closed();
            return;
        }
    }

    void TriggersWindow::update(float delta)
    {
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
        if (ImGui::BeginChild("Triggers List", ImVec2(220, 0), true))
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

            if (ImGui::BeginCombo("##commandfilter", _all_commands[_selected_command].c_str()))
            {
                for (int n = 0; n < _all_commands.size(); ++n)
                {
                    bool is_selected = _selected_command == n;
                    if (ImGui::Selectable(_all_commands[n].c_str(), is_selected))
                    {
                        _selected_commands.clear();
                        if (_all_commands[n] == "Flipmaps")
                        {
                            _selected_commands.push_back(TriggerCommandType::FlipMap);
                            _selected_commands.push_back(TriggerCommandType::FlipOff);
                            _selected_commands.push_back(TriggerCommandType::FlipOn);
                        }
                        else if (_all_commands[n] != "All")
                        {
                            _selected_commands.push_back(command_from_name(_all_commands[n]));
                        }
                        _selected_command = n;
                    }

                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

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
                    std::sort(_all_triggers.begin(), _all_triggers.end(),
                        [&](const auto& l, const auto& r) -> int
                        {
                            const auto l_l = l.lock();
                            const auto r_l = r.lock();

                            switch (specs->Specs[0].ColumnIndex)
                            {
                            case 0:
                                return specs->Specs->SortDirection == ImGuiSortDirection_Ascending
                                    ? (l_l->number() < r_l->number()) : (l_l->number() > r_l->number());
                            case 1:
                                return specs->Specs->SortDirection == ImGuiSortDirection_Ascending
                                    ? (l_l->room() < r_l->room()) : (l_l->room() > r_l->room());
                            case 2:
                                return specs->Specs->SortDirection == ImGuiSortDirection_Ascending
                                    ? (l_l->type() < r_l->type()) : (l_l->type() > r_l->type());
                            case 3:
                                return specs->Specs->SortDirection == ImGuiSortDirection_Ascending
                                    ? (l_l->visible() < r_l->visible()) : (l_l->visible() > r_l->visible());
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

                    if (!_selected_commands.empty() && !has_any_command(*trigger_ptr, _selected_commands))
                    {
                        continue;
                    }

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    bool selected = _selected_trigger.lock() && _selected_trigger.lock()->number() == trigger_ptr->number();
                    if (selected && _scroll_to_trigger)
                    {
                        const auto pos = ImGui::GetCurrentWindow()->DC.CursorPos;
                        if (!ImGui::IsRectVisible(pos, pos + ImVec2(1, 1)))
                        {
                            ImGui::SetScrollHereY();
                        }
                        _scroll_to_trigger = false;
                    }
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
        ImGui::EndChild();
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

        if (ImGui::BeginChild("Trigger Details", ImVec2(), true))
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
                        if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_SelectOnNav))
                        {
                            _clipboard->write(_window, to_utf16(value));
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
            ImGui::Text("Commands");
            if (ImGui::BeginTable("##commands", 3, ImGuiTableFlags_ScrollY, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupColumn("Index");
                ImGui::TableSetupColumn("Entity");
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
                        if (ImGui::Selectable((to_utf8(command_type_name(command.type())) + "##" + std::to_string(command.number())).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_SelectOnNav))
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
        ImGui::EndChild();
    }

    bool TriggersWindow::render_triggers_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(520, 500));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            render_triggers_list();
            ImGui::SameLine();
            render_trigger_details();
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }
}
