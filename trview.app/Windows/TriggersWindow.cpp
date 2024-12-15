#include "TriggersWindow.h"
#include <trview.common/Strings.h>
#include "../trview_imgui.h"
#include <format>
#include "RowCounter.h"
#include "../Elements/IRoom.h"
#include "../Elements/ILevel.h"

namespace trview
{
    ITriggersWindow::~ITriggersWindow()
    {
    }

    TriggersWindow::TriggersWindow(const std::shared_ptr<IClipboard>& clipboard)
        : _clipboard(clipboard)
    {
        setup_filters();

        _token_store += _track.on_toggle<Type::Room>() += [&](bool value)
        {
            _need_filtering = true;
            if (value)
            {
                set_current_room(_current_room);
            }
            else
            {
                _filter_applied = false;
                set_triggers(_all_triggers);
            }
        };
    }

    void TriggersWindow::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _all_triggers = triggers;

        std::set<TriggerCommandType> command_set;
        for (const auto& trigger : triggers)
        {
            const auto trigger_ptr = trigger.lock();
            for (const auto& command : trigger_ptr->commands())
            {
                command_set.insert(command.type());
            }
        }
        std::vector<VirtualCommand> all_commands{ { .name = "All" }, { .name = "Flipmaps" } };
        all_commands.append_range(command_set | std::views::transform([](auto&& c) -> VirtualCommand { return { .name = command_type_name(c), .type = c }; }));
        _all_commands = all_commands;

        setup_filters();
        _need_filtering = true;
        calculate_column_widths();
    }

    void TriggersWindow::clear_selected_trigger()
    {
        _selected_trigger.reset();
        _local_selected_trigger_commands.clear();
    }

    void TriggersWindow::set_current_room(const std::weak_ptr<IRoom>& room)
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

    void TriggersWindow::set_items(const std::vector<std::weak_ptr<IItem>>& items)
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
        calculate_column_widths();
        if (ImGui::BeginChild(Names::trigger_list_panel.c_str(), ImVec2(0, 0), ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoScrollbar))
        {
            _auto_hider.check_focus();

            _filters.render();
            ImGui::SameLine();

            _track.render();
            ImGui::SameLine();
            bool sync_trigger = _sync_trigger;
            if (ImGui::Checkbox(Names::sync_trigger.c_str(), &sync_trigger))
            {
                set_sync_trigger(sync_trigger);
            }

            _auto_hider.render();

            ImGui::PushItemWidth(-1);
            const auto current_command = _selected_command.value_or(VirtualCommand{ .name = "All" });
            if (ImGui::BeginCombo(Names::command_filter.c_str(), current_command.name.c_str()))
            {
                for (const auto& command : _all_commands)
                {
                    bool is_selected = current_command == command;
                    if (ImGui::Selectable(command.name.c_str(), is_selected))
                    {
                        _selected_commands.clear();
                        if (command.name == "Flipmaps")
                        {
                            _selected_commands.push_back(TriggerCommandType::FlipMap);
                            _selected_commands.push_back(TriggerCommandType::FlipOff);
                            _selected_commands.push_back(TriggerCommandType::FlipOn);
                        }
                        else if (command.name != "All")
                        {
                            _selected_commands.push_back(command.type);
                        }
                        _selected_command = command;
                        _need_filtering = true;
                    }

                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            RowCounter counter{ "triggers", _all_triggers.size() };
            if (ImGui::BeginTable(Names::triggers_list.c_str(), 4, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY, ImVec2(0, -counter.height())))
            {
                imgui_header_row(
                    {
                        { "#", _column_sizer.size(0) },
                        { "Room", _column_sizer.size(1) },
                        { "Type", _column_sizer.size(2) },
                        { .name = "Hide", .width = _column_sizer.size(3), .set_checked = [&](bool v)
                            {
                                std::ranges::for_each(_filtered_triggers, [=](auto&& trigger) 
                                    {
                                        auto trigger_ptr = trigger.lock(); trigger_ptr->set_visible(!v); 
                                    });
                                on_scene_changed();
                            }, .checked = std::ranges::all_of(_filtered_triggers, [](auto&& trigger) { auto trigger_ptr = trigger.lock(); return trigger_ptr ? !trigger_ptr->visible() : false; })
                        }
                    });

                filter_triggers();
                counter.set_count(_filtered_triggers.size());

                imgui_sort_weak(_filtered_triggers,
                    {
                        [](auto&& l, auto&& r) { return l.number() < r.number(); },
                        [](auto&& l, auto&& r) { return std::tuple(trigger_room(l), l.number()) < std::tuple(trigger_room(r), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(trigger_type_name(l.type()), l.number()) < std::tuple(trigger_type_name(r.type()), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.visible(), l.number()) < std::tuple(r.visible(), r.number()); }
                    }, _force_sort);

                ImGuiListClipper clipper;
                clipper.Begin(static_cast<int>(std::ssize(_filtered_triggers)));

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

                        ImGui::SetNextItemAllowOverlap();
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

                        ImGui::TableNextColumn();
                        ImGui::Text(std::to_string(trigger_room(trigger_ptr)).c_str());
                        ImGui::TableNextColumn();
                        ImGui::Text(trigger_type_name(trigger_ptr->type()).c_str());
                        ImGui::TableNextColumn();
                        bool hidden = !trigger_ptr->visible();
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                        if (ImGui::Checkbox(std::format("##hide-{}", trigger_ptr->number()).c_str(), &hidden))
                        {
                            trigger_ptr->set_visible(!hidden);
                            on_scene_changed();
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
                counter.render();
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
                    if (auto item = _all_items[command.index()].lock())
                    {
                        return item->type();
                    }
                }
                return std::string("No Item");
            }
            return std::string();
        };

        if (ImGui::BeginChild(Names::details_panel.c_str(), ImVec2(), true))
        {
            ImGui::Text("Trigger Details");
            auto selected_trigger = _selected_trigger.lock();
            if (ImGui::BeginTable(Names::trigger_stats.c_str(), 2, 0, ImVec2(-1, 150)))
            {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Value");
                ImGui::TableNextRow();

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
                    add_stat("Room", trigger_room(selected_trigger));
                    add_stat("Flags", format_binary(selected_trigger->flags()));
                    add_stat("Only once", selected_trigger->only_once());
                    add_stat("Timer", selected_trigger->timer());

                    ImGui::TableNextColumn();
                    ImGui::Text("Colour");
                    ImGui::TableNextColumn();

                    if (ImGui::Button(Names::reset_colour.c_str()))
                    {
                        selected_trigger->set_colour(std::nullopt);
                    }
                    ImGui::SameLine();
                    DirectX::SimpleMath::Color colour = selected_trigger->colour();
                    if (ImGui::ColorEdit4(Names::colour.c_str(), &colour.x, ImGuiColorEditFlags_NoInputs))
                    {
                        selected_trigger->set_colour(colour);
                    }
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
                            _track.set_enabled<Type::Room>(false);
                            on_item_selected(_all_items[command.index()]);
                        }
                        else if (equals_any(command.type(), TriggerCommandType::UnderwaterCurrent, TriggerCommandType::Camera))
                        {
                            _track.set_enabled<Type::Room>(false);
                            if (selected_trigger)
                            {
                                if (auto level = selected_trigger->level().lock())
                                {
                                    on_camera_sink_selected(level->camera_sink(command.index()));
                                }
                            }
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
        _filters.add_getter<float>("Room", [](auto&& trigger) { return static_cast<float>(trigger_room(trigger)); });
        _filters.add_getter<std::string>("Flags", [](auto&& trigger) { return format_binary(trigger.flags()); });
        _filters.add_getter<bool>("Only once", [](auto&& trigger) { return trigger.only_once(); });
        _filters.add_getter<float>("Timer", [](auto&& trigger) { return static_cast<float>(trigger.timer()); });

        _filters.add_multi_getter<std::string>("Command", [=](auto&& trigger)
            {
                return trigger.commands()
                    | std::views::transform([](auto&& t) { return command_type_name(t.type()); })
                    | std::ranges::to<std::vector>();
            });

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
        if (!_need_filtering && !_filters.test_and_reset_changed() && !_auto_hider.changed())
        {
            return;
        }

        _filtered_triggers.clear();
        std::copy_if(_all_triggers.begin(), _all_triggers.end(), std::back_inserter(_filtered_triggers),
            [&](const auto& trigger)
            {
                const auto trigger_ptr = trigger.lock();
                return !((_track.enabled<Type::Room>() && trigger_ptr->room().lock() != _current_room.lock() || !_filters.match(*trigger_ptr)) ||
                         (!_selected_commands.empty() && !has_any_command(*trigger_ptr, _selected_commands)));
            });
        _need_filtering = false;
        _force_sort = true;

        if (_auto_hider.apply(_all_triggers, _filtered_triggers | std::views::transform([](auto&& t) { return t.lock(); })))
        {
            on_scene_changed();
        }
    }

    void TriggersWindow::calculate_column_widths()
    {
        if (ImGui::GetCurrentContext() == nullptr)
        {
            return;
        }

        _column_sizer.reset();
        _column_sizer.measure("#__", 0);
        _column_sizer.measure("Room__", 1);
        _column_sizer.measure("Type__", 2);
        _column_sizer.measure("Hide____", 3);
        for (const auto& trigger : _all_triggers)
        {
            const auto trigger_ptr = trigger.lock();
            if (trigger_ptr)
            {
                _column_sizer.measure(std::to_string(trigger_ptr->number()), 0);
                if (auto room = trigger_ptr->room().lock())
                {
                    _column_sizer.measure(std::to_string(room->number()), 1);
                }
                _column_sizer.measure(trigger_type_name(trigger_ptr->type()), 2);
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
                return static_cast<int>(found - _filtered_triggers.begin());
            }
        }
        return std::nullopt;
    }

    bool TriggersWindow::VirtualCommand::operator == (const VirtualCommand& other) const noexcept
    {
        return name == other.name;
    }
}
