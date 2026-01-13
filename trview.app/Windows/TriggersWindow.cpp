#include "TriggersWindow.h"
#include <trview.common/Strings.h>
#include "../trview_imgui.h"
#include <format>
#include "RowCounter.h"
#include "../Elements/IRoom.h"
#include "../Elements/ILevel.h"
#include "../Messages/Messages.h"

namespace trview
{
    namespace
    {
        std::shared_ptr<ISector> sector_for_item(auto&& item)
        {
            if (const auto room = item->room().lock())
            {
                const auto pos = item->position() - room->position();
                return room->sector(static_cast<int32_t>(pos.x), static_cast<int32_t>(pos.z)).lock();
            }
            return {};
        }

        std::string soundtrack_id_to_file(uint16_t index)
        {
            std::optional<uint16_t> new_index;
            if (index >= 3 && index <= 23)
            {
                new_index = static_cast<uint16_t>(index - 1);
            }
            else if (index >= 27)
            {
                new_index = static_cast<uint16_t>(index - 4);
            }
            return new_index.has_value() ? std::to_string(new_index.value()) : "?";
        }
    }

    TriggersWindow::TriggersWindow(const std::shared_ptr<IClipboard>& clipboard, const std::weak_ptr<IMessageSystem>& messaging)
        : _clipboard(clipboard), _messaging(messaging)
    {
        setup_filters();

        _filters.set_columns(std::vector<std::string>{ "#", "Room", "Type", "Hide" });
        _token_store += _filters.on_columns_reset += [this]()
            {
                _filters.set_columns(std::vector<std::string>{ "#", "Room", "Type", "Hide" });
            };
        _token_store += _filters.on_columns_saved += [this]()
            {
                if (_settings)
                {
                    _settings->triggers_window_columns = _filters.columns();
                    messages::send_settings(_messaging, *_settings);
                }
            };

        _token_store += _track.on_toggle<Type::Room>() += [&](bool value)
        {
            if (value)
            {
                set_current_room(_current_room);
            }
            else
            {
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
    }

    void TriggersWindow::clear_selected_trigger()
    {
        _selected_trigger.reset();
        _local_selected_trigger_commands.clear();
    }

    void TriggersWindow::set_current_room(const std::weak_ptr<IRoom>& room)
    {
        _current_room = room;
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
            _filters.scroll_to_item();
            set_local_selected_trigger(trigger);
        }
    }

    void TriggersWindow::set_sync_trigger(bool value)
    {
        if (_sync_trigger != value)
        {
            _sync_trigger = value;
            _filters.scroll_to_item();
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

    void TriggersWindow::set_platform_and_version(const trlevel::PlatformAndVersion& platform_and_version)
    {
        _platform_and_version = platform_and_version;
    }

    std::weak_ptr<ITrigger> TriggersWindow::selected_trigger() const
    {
        return _selected_trigger;
    }

    void TriggersWindow::render()
    {
        if (!_settings)
        {
            messages::get_settings(_messaging, weak_from_this());
        }

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
            ImGui::SameLine();
            _filters.render_settings();

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
                    }

                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            auto filtered_triggers = 
                _all_triggers | 
                std::views::transform([](auto&& t) { return t.lock(); }) |
                std::views::filter([&](auto&& t)
                    {
                        return !((_track.enabled<Type::Room>() && t->room().lock() != _current_room.lock() || !_filters.match(*t)) ||
                                (!_selected_commands.empty() && !has_any_command(*t, _selected_commands)));
                    }) |
                std::ranges::to<std::vector>();

            _auto_hider.apply(_all_triggers, filtered_triggers, _filters);

            RowCounter counter{ "trigger", _all_triggers.size() };
            _filters.render_table(filtered_triggers, _all_triggers, _selected_trigger, counter,
                [&](auto&& trigger)
                {
                    set_local_selected_trigger(trigger);
                    if (_sync_trigger)
                    {
                        messages::send_select_trigger(_messaging, trigger);
                    }
                }, default_hide(filtered_triggers));
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
                    auto add_stat = [&]<typename T>(const std::string & name, const T && value)
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

                    add_stat("Type", to_string(selected_trigger->type()));
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

            if (!_local_selected_trigger_trigger_triggerers.empty() && ImGui::BeginChild("Trigger triggerer", ImVec2(), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY))
            {
                for (const auto& item : _local_selected_trigger_trigger_triggerers)
                {
                    if (auto item_ptr = item.lock())
                    {
                        ImGui::TextWrapped("This trigger is affected by a trigger triggerer. This trigger will be disabled until the item is activated.");
                        if (ImGui::Button(std::format("Item {} - {}", item_ptr->number(), item_ptr->type()).c_str(), ImVec2(-1, 0)))
                        {
                            messages::send_select_item(_messaging, item);
                        }
                    }
                }
                ImGui::EndChild();
            }

            if (ImGui::Button(Names::add_to_route.c_str(), ImVec2(-1, 30)))
            {
                messages::send_add_to_route(_messaging, _selected_trigger);
            }

            const bool any_extra = std::ranges::any_of(_local_selected_trigger_commands, [](auto&& c) { return c.data().size() > 1; });

            ImGui::Text("Commands");
            if (ImGui::BeginTable(Names::commands_list.c_str(), any_extra ? 5 : 4, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingFixedFit, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupColumn("Index");
                ImGui::TableSetupColumn("Entity");
                if (any_extra)
                {
                    ImGui::TableSetupColumn("Extra");
                }
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableHeadersRow();

                imgui_sort(_local_selected_trigger_commands,
                    {
                        [](auto&& l, auto&& r) { return l.number() < r.number(); },
                        [](auto&& l, auto&& r) { return std::tuple(command_type_name(l.type()), l.index()) < std::tuple(command_type_name(r.type()), r.index()); },
                        [](auto&& l, auto&& r) { return l.index() < r.index(); },
                        [&](auto&& l, auto&& r) { return std::tuple(get_command_display(l), l.index()) < std::tuple(get_command_display(r), r.index()); },
                    }, _force_sort);

                for (auto& command : _local_selected_trigger_commands)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    bool selected = false;
                    if (ImGui::Selectable(std::format("{}##command-{}", command.number(), command.number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                    {
                        if (command.type() == TriggerCommandType::LookAtItem || command.type() == TriggerCommandType::Object && command.index() < _all_items.size())
                        {
                            _track.set_enabled<Type::Room>(false);
                            messages::send_select_item(_messaging, _all_items[command.index()]);
                        }
                        else if (equals_any(command.type(), TriggerCommandType::UnderwaterCurrent, TriggerCommandType::Camera))
                        {
                            _track.set_enabled<Type::Room>(false);
                            if (selected_trigger)
                            {
                                if (auto level = selected_trigger->level().lock())
                                {
                                    messages::send_select_camera_sink(_messaging, level->camera_sink(command.index()));
                                }
                            }
                        }
                        else if (command.type() == TriggerCommandType::Flyby)
                        {
                            _track.set_enabled<Type::Room>(false);
                            if (selected_trigger)
                            {
                                if (auto level = selected_trigger->level().lock())
                                {
                                    for (const auto flyby : level->flybys() | std::views::transform([](auto&& f) { return f.lock(); }))
                                    {
                                        if (flyby && flyby->number() == command.index())
                                        {
                                            const auto nodes = flyby->nodes();
                                            if (!nodes.empty())
                                            {
                                                messages::send_select_flyby_node(_messaging, nodes[0]);
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    ImGui::TableNextColumn();
                    ImGui::Text(command_type_name(command.type()).c_str());
                    ImGui::TableNextColumn();
                    if (command.type() == TriggerCommandType::PlaySoundtrack &&
                        _platform_and_version.version == trlevel::LevelVersion::Tomb2)
                    {
                        ImGui::Text(std::format("{} ({})", command.index(), soundtrack_id_to_file(command.index())).c_str());
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::BeginTooltip();
                            ImGui::Text("Actual track file number is in parenthesis");
                            ImGui::EndTooltip();
                        }
                    }
                    else
                    {
                        ImGui::Text(std::to_string(command.index()).c_str());
                    }
                    ImGui::TableNextColumn();
                    ImGui::Text(get_command_display(command).c_str());;
                    if (any_extra)
                    {
                        ImGui::TableNextColumn();
                        const auto data = command.data();
                        ImGui::Text(join(
                            std::ranges::subrange(data.begin() + 1, data.end()) |
                            std::views::transform([](auto&& d) { return std::to_string(d); })).c_str());
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
        _local_selected_trigger_trigger_triggerers.clear();
        if (auto selected_trigger = _selected_trigger.lock())
        {
            _local_selected_trigger_commands = selected_trigger->commands();
            find_trigger_triggerers();
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
                available_types.insert(to_string(trigger_ptr->type()));
            }
        }
        _filters.add_getter<std::string>("Type", { available_types.begin(), available_types.end() }, [](auto&& trigger) { return to_string(trigger.type()); });
        _filters.add_getter<int>("#", [](auto&& trigger) { return static_cast<int>(trigger.number()); });
        _filters.add_getter<int>("Room", [](auto&& trigger) { return static_cast<int>(trigger_room(trigger)); });
        _filters.add_getter<std::string>("Flags", [](auto&& trigger) { return format_binary(trigger.flags()); });
        _filters.add_getter<bool>("Only once", [](auto&& trigger) { return trigger.only_once(); });
        _filters.add_getter<int>("Timer", [](auto&& trigger) { return static_cast<int>(trigger.timer()); });
        _filters.add_getter<bool>("Hide", [](auto&& trigger) { return !trigger.visible(); }, EditMode::ReadWrite);
        _filters.add_getter<bool>("In Visible Room", [](auto&& trigger)
            {
                if (const auto level = trigger.level().lock())
                {
                    return level->is_in_visible_set(trigger.room());
                }
                return false;
            });

        _filters.add_multi_getter<std::string>("Command", [=](auto&& trigger)
            {
                return trigger.commands()
                    | std::views::transform([](auto&& t) { return command_type_name(t.type()); })
                    | std::ranges::to<std::vector>();
            });

        _filters.add_multi_getter<float>("Trigger triggerer", [&](auto&& trigger)
            {
                const auto sector = trigger.sector().lock();
                return _all_items
                    | std::views::filter([&](const auto& i)
                      {
                          auto item = i.lock();
                          return item && item->type() == "Trigger triggerer" && sector_for_item(item) == sector;
                      })
                    | std::views::transform([](const auto& i) -> std::shared_ptr<IItem> { return i.lock(); })
                    | std::views::filter([](const auto& i) { return i != nullptr; })
                    | std::views::transform([](const auto& i) { return static_cast<float>(i->number()); })
                    | std::ranges::to<std::vector>();
            });

        _filters.add_multi_getter<float>("Extra", [&](auto&& trigger)
            {
                return trigger.commands()
                    | std::views::transform([](auto&& t) -> std::vector<float>
                        {
                            const auto data = t.data();
                            return data.size() < 2 ? std::vector<float>{} : (
                                      std::ranges::subrange(data.begin() + 1, data.end())
                                    | std::views::transform([](auto&& d) { return static_cast<float>(d); })
                                    | std::ranges::to<std::vector>());
                        })
                    | std::views::join
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

    bool TriggersWindow::VirtualCommand::operator == (const VirtualCommand& other) const noexcept
    {
        return name == other.name;
    }

    void TriggersWindow::find_trigger_triggerers()
    {
        if (const auto trigger = _selected_trigger.lock())
        {
            if (const auto sector = trigger->sector().lock())
            {
                _local_selected_trigger_trigger_triggerers = _all_items |
                    std::views::filter([&](auto&& i)
                        {
                            auto item = i.lock();
                            return item && item->type() == "Trigger triggerer" && sector_for_item(item) == sector;
                        }) |
                    std::ranges::to<std::vector>();
            }
        }
    }

    void TriggersWindow::receive_message(const Message& message)
    {
        if (auto settings = messages::read_settings(message))
        {
            _settings = settings.value();
            if (!_columns_set)
            {
                _filters.set_columns(_settings->triggers_window_columns);
                _columns_set = true;
            }
        }
        else if (auto selected_trigger = messages::read_select_trigger(message))
        {
            set_selected_trigger(selected_trigger.value());
        }
        else if (auto selected_room = messages::read_select_room(message))
        {
            set_current_room(selected_room.value());
        }
        else if (auto level = messages::read_open_level(message))
        {
            if (auto level_ptr = level->lock())
            {
                set_items(level_ptr->items());
                set_triggers(level_ptr->triggers());
                set_platform_and_version(level_ptr->platform_and_version());
            }
        }
    }

    void TriggersWindow::initialise()
    {
        messages::get_open_level(_messaging, weak_from_this());
        messages::get_selected_room(_messaging, weak_from_this());
        messages::get_selected_trigger(_messaging, weak_from_this());
    }

    std::string TriggersWindow::type() const
    {
        return "Triggers";
    }

    std::string TriggersWindow::title() const
    {
        return _id;
    }
}
