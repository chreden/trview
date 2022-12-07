#include "TriggersWindow.h"
#include <trview.common/Strings.h>
#include "../trview_imgui.h"
#include <format>

namespace trview
{
    ITriggersWindow::~ITriggersWindow()
    {
    }

    TriggersWindow::TriggersWindow(const std::shared_ptr<IClipboard>& clipboard)
        : _clipboard(clipboard)
    {
        setup_filters();
    }

    void TriggersWindow::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _all_triggers = triggers;

        const std::string existing_command = _selected_command < _all_commands.size() ? _all_commands[_selected_command] : "All";

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
        std::transform(command_set.begin(), command_set.end(), std::back_inserter(all_commands), command_type_name);
        _all_commands = all_commands;

        auto reselected_command = std::find(_all_commands.begin(), _all_commands.end(), existing_command);
        if (reselected_command != _all_commands.end())
        {
            _selected_command = reselected_command - _all_commands.begin();
        }

        setup_filters();
        _need_filtering = true;
        calculate_column_widths();
    }

    void TriggersWindow::clear_selected_trigger()
    {
        _selected_trigger.reset();
        _local_selected_trigger_commands.clear();
    }

    void TriggersWindow::set_current_room(uint32_t room)
    {
        _current_room = room;
        _need_filtering = true;
    }

    void TriggersWindow::set_number(int32_t number)
    {
        _id = "Triggers " + std::to_string(number);
    }

    void TriggersWindow::set_selected_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        _global_selected_trigger = trigger;
        if (_sync_trigger)
        {
            _scroll_to_trigger = true;
            set_local_selected_trigger(trigger);
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
            _need_filtering = true;
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

    void TriggersWindow::render()
    {
        if (!render_triggers_window())
        {
            on_window_closed();
            return;
        }
    }

    void TriggersWindow::update(float delta)
    {
        if (_tooltip_timer.has_value())
        {
            _tooltip_timer = _tooltip_timer.value() + delta;
            if (_tooltip_timer.value() > 0.6f)
            {
                _tooltip_timer.reset();
            }
        }
    }

    void TriggersWindow::render_triggers_list()
    {
        if (ImGui::BeginChild(Names::trigger_list_panel.c_str(), ImVec2(220, 0), true))
        {
            _filters.render();
            ImGui::SameLine();

            bool track_room = _track_room;
            if (ImGui::Checkbox(Names::track_room.c_str(), &track_room))
            {
                set_track_room(track_room);
            }
            ImGui::SameLine();
            bool sync_trigger = _sync_trigger;
            if (ImGui::Checkbox(Names::sync_trigger.c_str(), &sync_trigger))
            {
                set_sync_trigger(sync_trigger);
            }

            ImGui::PushItemWidth(-1);
            std::string preview_command = _selected_command < _all_commands.size() ? _all_commands[_selected_command] : "";
            if (ImGui::BeginCombo(Names::command_filter.c_str(), preview_command.c_str()))
            {
                for (std::size_t n = 0; n < _all_commands.size(); ++n)
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
                        _need_filtering = true;
                    }

                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::BeginTable(Names::triggers_list.c_str(), 4, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, _required_number_width);
                ImGui::TableSetupColumn("Room");
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, _required_type_width);
                ImGui::TableSetupColumn("Hide");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                filter_triggers();

                imgui_sort_weak(_filtered_triggers,
                    {
                        [](auto&& l, auto&& r) { return l.number() < r.number(); },
                        [](auto&& l, auto&& r) { return std::tuple(l.room(), l.number()) < std::tuple(r.room(), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(trigger_type_name(l.type()), l.number()) < std::tuple(trigger_type_name(r.type()), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.visible(), l.number()) < std::tuple(r.visible(), r.number()); }
                    }, _force_sort);

                ImGuiListClipper clipper;
                clipper.Begin(std::ssize(_filtered_triggers));

                while (clipper.Step())
                {
                    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                    {
                        const auto trigger_ptr = _filtered_triggers[i].lock();

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        bool selected = _selected_trigger.lock() && _selected_trigger.lock()->number() == trigger_ptr->number();

                        ImGuiScroller scroller;
                        if (selected && _scroll_to_trigger)
                        {
                            scroller.scroll_to_item();
                            _scroll_to_trigger = false;
                        }

                        
                        if (ImGui::Selectable(std::format("{0}##{0}", trigger_ptr->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                        {
                            scroller.fix_scroll();
                            set_local_selected_trigger(trigger_ptr);
                            if (_sync_trigger)
                            {
                                on_trigger_selected(trigger_ptr);
                            }
                            _scroll_to_trigger = false;
                        }

                        ImGui::SetItemAllowOverlap();
                        ImGui::TableNextColumn();
                        ImGui::Text(std::to_string(trigger_ptr->room()).c_str());
                        ImGui::TableNextColumn();
                        ImGui::Text(trigger_type_name(trigger_ptr->type()).c_str());
                        ImGui::TableNextColumn();
                        bool hidden = !trigger_ptr->visible();
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                        if (ImGui::Checkbox(std::format("##hide-{}", trigger_ptr->number()).c_str(), &hidden))
                        {
                            on_trigger_visibility(trigger_ptr, !hidden);
                        }
                        ImGui::PopStyleVar();
                    }
                }

                const auto index = index_of_selected();
                if (index.has_value())
                {
                    const float item_pos_y = clipper.StartPosY + clipper.ItemsHeight * index.value();
                    ImGui::SetScrollFromPosY(item_pos_y - ImGui::GetWindowPos().y);
                }

                clipper.End();
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
                return std::string("No Item");
            }
            return std::string();
        };

        if (ImGui::BeginChild(Names::details_panel.c_str(), ImVec2(), true))
        {
            ImGui::Text("Trigger Details");
            if (ImGui::BeginTable(Names::trigger_stats.c_str(), 2, 0, ImVec2(-1, 150)))
            {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Value");
                ImGui::TableNextRow();

                auto selected_trigger = _selected_trigger.lock();
                if (selected_trigger)
                {
                    auto add_stat = [&]<typename T>(const std::string& name, const T&& value)
                    {
                        const auto string_value = get_string(value);
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                        {
                            _clipboard->write(to_utf16(string_value));
                            _tooltip_timer = 0.0f;
                        }
                        ImGui::TableNextColumn();
                        ImGui::Text(string_value.c_str());
                    };

                    auto position_text = [=]()
                    {
                        const auto p = selected_trigger->position() * trlevel::Scale;
                        return std::format("{:.0f}, {:.0f}, {:.0f}", p.x, p.y, p.z);
                    };

                    add_stat("Type", trigger_type_name(selected_trigger->type()));
                    add_stat("#", selected_trigger->number());
                    add_stat("Position", position_text());
                    add_stat("Room", selected_trigger->room());
                    add_stat("Flags", format_binary(selected_trigger->flags()));
                    add_stat("Only once", selected_trigger->only_once());
                    add_stat("Timer", selected_trigger->timer());
                }

                ImGui::EndTable();
            }
            if (ImGui::Button(Names::add_to_route.c_str(), ImVec2(-1, 30)))
            {
                on_add_to_route(_selected_trigger);
            }
            ImGui::Text("Commands");
            if (ImGui::BeginTable(Names::commands_list.c_str(), 3, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingFixedFit, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupColumn("Index");
                ImGui::TableSetupColumn("Entity");
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                imgui_sort(_local_selected_trigger_commands,
                    {
                        [](auto&& l, auto&& r) { return std::tuple(command_type_name(l.type()), l.index()) < std::tuple(command_type_name(r.type()), r.index()); },
                        [](auto&& l, auto&& r) { return l.index() < r.index(); },
                        [&](auto&& l, auto&& r) { return std::tuple(get_command_display(l), l.index()) < std::tuple(get_command_display(r), r.index()); },
                    }, _force_sort);

                for (auto& command : _local_selected_trigger_commands)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    bool selected = false;
                    if (ImGui::Selectable(std::format("{}##{}", command_type_name(command.type()), command.number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
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
                    ImGui::Text(get_command_display(command).c_str());;
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
            _force_sort = false;

            if (_tooltip_timer.has_value())
            {
                ImGui::BeginTooltip();
                ImGui::Text("Copied");
                ImGui::EndTooltip();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    void TriggersWindow::set_local_selected_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        _selected_trigger = trigger;
        _local_selected_trigger_commands.clear();
        if (auto selected_trigger = _selected_trigger.lock())
        {
            _local_selected_trigger_commands = selected_trigger->commands();
            _need_filtering = true;
        }
    }

    void TriggersWindow::setup_filters()
    {
        _filters.clear_all_getters();
        std::set<std::string> available_types;
        for (const auto& trigger : _all_triggers)
        {
            if (auto trigger_ptr = trigger.lock())
            {
                available_types.insert(trigger_type_name(trigger_ptr->type()));
            }
        }
        _filters.add_getter<std::string>("Type", { available_types.begin(), available_types.end() }, [](auto&& trigger) { return trigger_type_name(trigger.type()); });
        _filters.add_getter<float>("#", [](auto&& trigger) { return static_cast<float>(trigger.number()); });
        _filters.add_getter<float>("Room", [](auto&& trigger) { return static_cast<float>(trigger.room()); });
        _filters.add_getter<std::string>("Flags", [](auto&& trigger) { return format_binary(trigger.flags()); });
        _filters.add_getter<bool>("Only once", [](auto&& trigger) { return trigger.only_once(); });
        _filters.add_getter<float>("Timer", [](auto&& trigger) { return static_cast<float>(trigger.timer()); });

        auto all_trigger_indices = [](TriggerCommandType type, const auto& trigger)
        {
            std::vector<float> indices;
            for (const auto& command : trigger.commands())
            {
                if (command.type() == type)
                {
                    indices.push_back(static_cast<float>(command.index()));
                }
            }
            return indices;
        };

        auto any_of_command = [&](TriggerCommandType type)
        {
            for (auto& trigger : _all_triggers)
            {
                if (auto trigger_ptr = trigger.lock())
                {
                    for (auto command : trigger_ptr->commands())
                    {
                        if (command.type() == type)
                        {
                            return true;
                        }
                    }
                }
            }
            return false;
        };

        auto add_multi_getter = [&](TriggerCommandType type)
        {
            if (any_of_command(type))
            {
                _filters.add_multi_getter<float>(command_type_name(type), [=](auto&& trigger) { return all_trigger_indices(type, trigger); });
            }
        };

        add_multi_getter(TriggerCommandType::Object);
        add_multi_getter(TriggerCommandType::Camera);
        add_multi_getter(TriggerCommandType::UnderwaterCurrent);
        add_multi_getter(TriggerCommandType::FlipMap);
        add_multi_getter(TriggerCommandType::FlipOn);
        add_multi_getter(TriggerCommandType::FlipOff);
        add_multi_getter(TriggerCommandType::LookAtItem);
        add_multi_getter(TriggerCommandType::EndLevel);
        add_multi_getter(TriggerCommandType::PlaySoundtrack);
        add_multi_getter(TriggerCommandType::Flipeffect);
        add_multi_getter(TriggerCommandType::SecretFound);
        add_multi_getter(TriggerCommandType::ClearBodies);
        add_multi_getter(TriggerCommandType::Flyby);
        add_multi_getter(TriggerCommandType::Cutscene);
    }

    void TriggersWindow::filter_triggers()
    {
        if (!_need_filtering && !_filters.test_and_reset_changed())
        {
            return;
        }

        _filtered_triggers.clear();
        std::copy_if(_all_triggers.begin(), _all_triggers.end(), std::back_inserter(_filtered_triggers),
            [&](const auto& trigger)
            {
                const auto trigger_ptr = trigger.lock();
                return !((_track_room && trigger_ptr->room() != _current_room || !_filters.match(*trigger_ptr)) ||
                         (!_selected_commands.empty() && !has_any_command(*trigger_ptr, _selected_commands)));
            });
        _need_filtering = false;
        _force_sort = true;
    }

    void TriggersWindow::calculate_column_widths()
    {
        if (ImGui::GetCurrentContext() == nullptr)
        {
            return;
        }

        _required_type_width = 0.0f;
        _required_number_width = 0.0f;
        for (const auto& trigger : _all_triggers)
        {
            const auto trigger_ptr = trigger.lock();
            if (trigger_ptr)
            {
                _required_number_width = std::max(_required_number_width,
                    ImGui::CalcTextSize(std::to_string(trigger_ptr->number()).c_str()).x);
                _required_type_width = std::max(_required_type_width,
                    ImGui::CalcTextSize(trigger_type_name(trigger_ptr->type()).c_str()).x);
            }
        }
    }

    std::optional<int> TriggersWindow::index_of_selected() const
    {
        const auto selected = _selected_trigger.lock();
        if (selected && _scroll_to_trigger)
        {
            const auto found = std::find_if(_filtered_triggers.begin(), _filtered_triggers.end(),
                [&](auto&& t)
                {
                    auto t_l = t.lock();
                    return t_l && t_l->number() == selected->number();
                });
            if (found != _filtered_triggers.end())
            {
                return found - _filtered_triggers.begin();
            }
        }
        return std::nullopt;
    }
}
