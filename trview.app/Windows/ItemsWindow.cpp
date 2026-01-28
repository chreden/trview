#include "ItemsWindow.h"
#include <trview.common/Strings.h>
#include <trview.common/Windows/Clipboard.h>
#include "../trview_imgui.h"
#include "RowCounter.h"
#include "../Elements/IRoom.h"
#include "../Elements/ISector.h"
#include "../Messages/Messages.h"
#include "../Elements/ILevel.h"
#include "../Elements/ElementFilters.h"
#include "../Elements/ITrigger.h"

namespace trview
{
    int32_t bound_rotation(int32_t v)
    {
        constexpr int32_t low = 0;
        constexpr int32_t high = 65535;
        constexpr int32_t size = high - low;
        if (v > high)
        {
            const int32_t extra = v - high;
            return low + extra - (extra / size) * size;
        }
        else if (v < low)
        {
            const int32_t extra = low - v;
            return high - (extra - (extra / size) * size);
        }
        return v;
    }

    void add_item_filters(Filters& filters,
        const std::set<std::string>& available_types,
        const std::set<std::string>& available_categories)
    {
        if (filters.has_type_key("IItem"))
        {
            return;
        }

        auto getters = Filters::GettersBuilder()
            .with_type_key("IItem")
            .with_getter<IItem, int>("#", [](auto&& item) { return static_cast<int>(item.number()); })
            .with_getter<IItem, std::string>("Type", { available_types.begin(), available_types.end() }, [](auto&& item) { return item.type(); })
            .with_multi_getter<IItem, std::string>("Category", { available_categories.begin(), available_categories.end() }, [](auto&& item)
                {
                    std::vector<std::string> results;
                    for (const auto& category : item.categories())
                    {
                        results.push_back(category);
                    }
                    return results;
                })
            .with_getter<IItem, int>("X", [](auto&& item) { return static_cast<int>(item.position().x * trlevel::Scale_X); })
            .with_getter<IItem, int>("Y", [](auto&& item) { return static_cast<int>(item.position().y * trlevel::Scale_Y); })
            .with_getter<IItem, int>("Z", [](auto&& item) { return static_cast<int>(item.position().z * trlevel::Scale_Z); })
            .with_getter<IItem, bool>("AI", [](auto&& item) { return item.is_ai(); })
            .with_getter<IItem, int>("Angle", [](auto&& item) { return static_cast<int>(bound_rotation(item.angle())); })
            .with_getter<IItem, int>("Angle Degrees", [](auto&& item) { return static_cast<int>(bound_rotation(item.angle()) / 182); })
            .with_getter<IItem, int>("Type ID", [](auto&& item) { return static_cast<int>(item.type_id()); }, EditMode::Read)
            .with_getter<IItem, int>("Room #", [](auto&& item) { return static_cast<int>(item_room(item)); }, EditMode::Read)
            .with_getter<IItem, std::weak_ptr<IFilterable>>("Room", {}, [](auto&& item) { return item.room(); }, {}, EditMode::Read, "IRoom")
            .with_getter<IItem, bool>("Clear Body", [](auto&& item) { return item.clear_body_flag(); })
            .with_getter<IItem, bool>("Invisible", [](auto&& item) { return item.invisible_flag(); })
            .with_getter<IItem, std::string>("Flags", [](auto&& item) { return format_binary(item.activation_flags()); })
            .with_getter<IItem, int>("OCB", [](auto&& item) { return static_cast<int>(item.ocb()); })
            .with_getter<IItem, bool>("Hide", [](auto&& item) { return !item.visible(); }, EditMode::ReadWrite)
            .with_getter<IItem, bool>("Remastered Extra", [](auto&& item) { return item.is_remastered_extra(); })
            .with_multi_getter<IItem, float>("Trigger References", [](auto&& item)
                {
                    std::vector<float> results;
                    for (auto trigger : item.triggers())
                    {
                        if (auto trigger_ptr = trigger.lock())
                        {
                            results.push_back(static_cast<float>(trigger_ptr->number()));
                        }
                    }
                    return results;
                })
            .with_multi_getter<IItem, std::weak_ptr<IFilterable>>("Trigger", {}, [](auto&& item) {  return item.triggers() |  std::ranges::to<std::vector<std::weak_ptr<IFilterable>>>(); }, {}, "ITrigger")
            .with_multi_getter<IItem, bool>("NG+", [](auto&& item)
                {
                    return item.ng_plus() == std::nullopt ? std::vector<bool>{} : std::vector<bool>{ false,true };
                })
            .with_getter<IItem, bool>("In Visible Room", [](auto&& item)
                {
                    if (const auto level = item.level().lock())
                    {
                        return level->is_in_visible_set(item.room());
                    }
                    return false;
                })
            .build<IItem>();

        filters.add_getters(getters);
    }

    ItemsWindow::ItemsWindow(const std::shared_ptr<IClipboard>& clipboard, const std::weak_ptr<IMessageSystem>& messaging)
        : _clipboard(clipboard), _messaging(messaging)
    {
        _tips["OCB"] = "Changes entity behaviour";
        _tips["Clear Body"] = "If true, removed when Bodybag is triggered";
        _tips["Trigger triggerer"] = "Disables the trigger on the same sector until this item is triggered";
        _tips["Type*"] = "Mutant Egg spawn target is missing; egg will be empty";
        _tips["Remastered Extra"] = "Item is added at runtime by the remasters, not present in level file";
        _tips["NG+"] = "Item only appears in NG+ mode in the remasters";

        setup_filters();

        _filters.set_columns(std::vector<std::string>{ "#", "Room #", "Type ID", "Type", "Hide" });
        _token_store += _filters.on_columns_reset += [this]()
            {
                _filters.set_columns(std::vector<std::string>{ "#", "Room #", "Type ID", "Type", "Hide" });
            };
        _token_store += _filters.on_columns_saved += [this]()
            {
                if (_settings)
                {
                    _settings->items_window_columns = _filters.columns();
                    messages::send_settings(_messaging, *_settings);
                }
            };
    }

    void ItemsWindow::set_items(const std::vector<std::weak_ptr<IItem>>& items)
    {
        _all_items = items;
        _triggered_by.clear();
        setup_filters();
        _force_sort = true;
        _filters.force_sort();
    }

    void ItemsWindow::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _all_triggers = triggers;
    }

    void ItemsWindow::clear_selected_item()
    {
        _selected_item.reset();
        _triggered_by.clear();
    }

    void ItemsWindow::set_current_room(const std::weak_ptr<IRoom>& room)
    {
        _current_room = room;
    }

    void ItemsWindow::set_sync_item(bool value)
    {
        if (_sync_item != value)
        {
            _sync_item = value;
            _filters.scroll_to_item();
            if (_sync_item && _global_selected_item.lock())
            {
                set_selected_item(_global_selected_item);
            }
        }
    }

    void ItemsWindow::set_selected_item(const std::weak_ptr<IItem>& item)
    {
        _global_selected_item = item;
        if (_sync_item)
        {
            _filters.scroll_to_item();
            set_local_selected_item(item);
        }
    }

    std::weak_ptr<IItem> ItemsWindow::selected_item() const
    {
        return _selected_item;
    }

    void ItemsWindow::render_items_list()
    {
        if (ImGui::BeginChild(Names::item_list_panel.c_str(), ImVec2(0, 0), ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoScrollbar))
        {
            _auto_hider.check_focus();
            _filters.render();
            ImGui::SameLine();
            _track.render();

            ImGui::SameLine();
            bool sync_item = _sync_item;
            if (ImGui::Checkbox(Names::sync_item.c_str(), &sync_item))
            {
                set_sync_item(sync_item);
            }

            _auto_hider.render();

            auto filtered_items = 
                _all_items | 
                std::views::transform([](auto&& item) { return item.lock(); }) |
                std::views::filter([&](auto&& item)
                    { 
                        return item && (!item->ng_plus().has_value() || item->ng_plus() == _ng_plus); 
                    }) |
                std::views::filter([&](auto&& item) 
                    {
                        return !(!item || (_track.enabled<Type::Room>() && item->room().lock() != _current_room.lock() || !_filters.match(*item)));
                    }) |
                std::ranges::to<std::vector>();

            _auto_hider.apply(_all_items, filtered_items, _filters.test_and_reset_changed());

            ImGui::SameLine();
            _filters.render_settings();

            RowCounter counter{ "item",
                static_cast<std::size_t>(std::ranges::count_if(_all_items, [this](auto&& item)
                    {
                        const auto item_ptr = item.lock();
                        return item_ptr && item_ptr->ng_plus().value_or(_ng_plus) == _ng_plus;
                    }))};
            _filters.render_table(filtered_items, _all_items, _selected_item, counter,
                [&](auto&& item)
                {
                    const std::shared_ptr<IItem> f_ptr = std::static_pointer_cast<IItem>(item.lock());
                    set_local_selected_item(f_ptr);
                    if (_sync_item)
                    {
                        messages::send_select_item(_messaging, f_ptr);
                    }
                }, default_hide(filtered_items));
        }
        ImGui::EndChild();
    }

    void ItemsWindow::render_item_details()
    {
        if (ImGui::BeginChild(Names::details_panel.c_str(), ImVec2(), ImGuiChildFlags_Border))
        {
            ImGui::Text("Item Details");
            if (ImGui::BeginTable(Names::item_stats.c_str(), 2, ImGuiTableFlags_SizingFixedFit, ImVec2(-1, 150)))
            {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Value");
                ImGui::TableNextRow();

                if (auto item = _selected_item.lock())
                {
                    auto add_stat = [&]<typename T>(const std::string& name, T&& value)
                    {
                        const auto string_value = get_string(value);
                        ImGui::TableNextColumn();
                        if (ImGui::Selectable(name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                        {
                            _clipboard->write(to_utf16(string_value));
                            _tooltip_timer = 0.0f;
                        }
                        if (ImGui::IsItemHovered() && _tips.find(name) != _tips.end())
                        {
                            ImGui::SetTooltip(_tips[name].c_str());
                        }
                        if (ImGui::IsItemHovered() && _tips.find(string_value) != _tips.end())
                        {
                            ImGui::SetTooltip(_tips[string_value].c_str());
                        }

                        ImGui::TableNextColumn();
                        ImGui::Text(string_value.c_str());
                    };

                    auto position_text = [&item]()
                    {
                        const auto pos = item->position() * trlevel::Scale;
                        return std::format("{:.0f}, {:.0f}, {:.0f}", pos.x, pos.y, pos.z);
                    };

                    const auto level = item->level().lock();
                    auto is_bad_mutant_egg = [&]() 
                    { 
                        return _level_version == trlevel::LevelVersion::Tomb1 &&
                            is_mutant_egg(*item) &&
                            level &&
                            level->has_model(mutant_egg_contents(*item));
                    };

                    add_stat(std::format("Type{}", is_bad_mutant_egg() ? "*" : ""), item->type());
                    add_stat("#", item->number());
                    add_stat("Position", position_text());
                    add_stat("AI", item->is_ai());
                    add_stat("Angle", bound_rotation(item->angle()));
                    add_stat("Angle Degrees", bound_rotation(item->angle()) / 182);
                    add_stat("Type ID", item->type_id());
                    add_stat("Room", item_room(item));
                    add_stat("Clear Body", item->clear_body_flag());
                    add_stat("Invisible", item->invisible_flag());
                    add_stat("Flags", format_binary(item->activation_flags()));
                    add_stat("OCB", item->ocb());
                    add_stat("Category", join(item->categories()));
                    if (item->is_remastered_extra())
                    {
                        add_stat("Remastered Extra", item->is_remastered_extra());
                    }
                    if (item->ng_plus().value_or(false))
                    {
                        add_stat("NG+", item->ng_plus().value());
                    }
                }

                ImGui::EndTable();
            }

            if (const auto item = _selected_item.lock())
            {
                const bool is_trigger_triggerer = item->type() == "Trigger triggerer";
                if (is_trigger_triggerer)
                {
                    if (ImGui::BeginChild("Trigger triggerer", ImVec2(), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY))
                    {
                        ImGui::TextWrapped("This item is a trigger triggerer for a trigger. The trigger will be disabled until this item is activated.");
                        if (const auto room = item->room().lock())
                        {
                            const auto pos = item->position() - room->position();
                            if (const auto sector = room->sector(static_cast<int32_t>(pos.x), static_cast<int32_t>(pos.z)).lock())
                            {
                                if (const auto trigger = sector->trigger().lock())
                                {
                                    if (ImGui::Button(std::format("{} {}", to_string(trigger->type()), trigger->number()).c_str(), ImVec2(-1,0)))
                                    {
                                        messages::send_select_trigger(_messaging, trigger);
                                    }
                                }
                            }
                        }
                        ImGui::EndChild();
                    }
                }
            }

            if (ImGui::Button(Names::add_to_route_button.c_str(), ImVec2(-1, 30)))
            {
                messages::send_add_to_route(_messaging, _selected_item);
            }

            render_trigger_references();
        }
        ImGui::EndChild();
    }

    bool ItemsWindow::render_items_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(540, 500));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            render_items_list();
            ImGui::SameLine();
            render_item_details();
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

    void ItemsWindow::set_filters(std::vector<Filters::Filter> filters)
    {
        filters;
        // TODO: Reinstate
        // _filters.set_filters(filters);
    }

    void ItemsWindow::render()
    {
        if (!_settings)
        {
            messages::get_settings(_messaging, weak_from_this());
        }

        if (!render_items_window())
        {
            on_window_closed();
            return;
        }
    }

    void ItemsWindow::update(float delta)
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

    void ItemsWindow::set_number(int32_t number)
    {
        _id = "Items " + std::to_string(number);
    }

    void ItemsWindow::set_local_selected_item(std::weak_ptr<IItem> item)
    {
        _selected_item = item;
        if (auto selected = _selected_item.lock())
        {
            _triggered_by = selected->triggers();
        }
        _force_sort = true;
        _filters.force_sort();
    }

    void ItemsWindow::setup_filters()
    {
        _filters.clear_all_getters();
        std::set<std::string> available_types;
        std::set<std::string> available_categories;
        for (const auto& item : _all_items)
        {
            if (auto item_ptr = item.lock())
            {
                available_types.insert(item_ptr->type());
                available_categories.insert_range(item_ptr->categories());
            }
        }

        add_item_filters(_filters, available_types, available_categories);
        add_room_filters(_filters, _level);
        add_trigger_filters(_filters, _level);

        _filters.set_type_key("IItem");
    }

    void ItemsWindow::set_level_version(trlevel::LevelVersion version)
    {
        _level_version = version;
    }

    void ItemsWindow::set_ng_plus(bool value)
    {
        _ng_plus = value;
    }

    std::string ItemsWindow::name() const
    {
        return _id;
    }

    void ItemsWindow::render_trigger_references()
    {
        ImGui::Text("Trigger References");
        if (ImGui::BeginTable(Names::triggers_list.c_str(), 3, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingFixedFit, ImVec2(-1, -1)))
        {
            ImGui::TableSetupColumn("#");
            ImGui::TableSetupColumn("Room");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            imgui_sort_weak(_triggered_by,
                {
                    [](auto&& l, auto&& r) { return l.number() < r.number(); },
                    [](auto&& l, auto&& r) { return std::tuple(trigger_room(l), l.number()) < std::tuple(trigger_room(r), r.number()); },
                    [](auto&& l, auto&& r) { return std::tuple(to_string(l.type()), l.number()) < std::tuple(to_string(r.type()), r.number()); },
                }, _force_sort);

            for (auto& trigger : _triggered_by)
            {
                auto trigger_ptr = trigger.lock();
                if (!trigger_ptr)
                {
                    continue;
                }

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                bool selected = _selected_trigger.lock() == trigger_ptr;
                if (ImGui::Selectable(std::to_string(trigger_ptr->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                {
                    _selected_trigger = trigger;
                    _track.set_enabled<Type::Room>(false);
                    messages::send_select_trigger(_messaging, trigger);
                }
                ImGui::TableNextColumn();
                ImGui::Text(std::to_string(trigger_room(trigger_ptr)).c_str());
                ImGui::TableNextColumn();
                ImGui::Text(to_string(trigger_ptr->type()).c_str());
            }
            ImGui::EndTable();
        }
    }

    void ItemsWindow::receive_message(const Message& message)
    {
        if (auto selected_item = messages::read_select_item(message))
        {
            set_selected_item(selected_item.value());
        }
        else if (auto selected_room = messages::read_select_room(message))
        {
            set_current_room(selected_room.value());
        }
        else if (auto settings = messages::read_settings(message))
        {
            _settings = settings.value();
            if (!_columns_set)
            {
                _filters.set_columns(_settings->items_window_columns);
                _columns_set = true;
            }
        }
        else if (auto level = messages::read_open_level(message))
        {
            if (auto level_ptr = level->lock())
            {
                _level = level.value();
                clear_selected_item();
                set_items(level_ptr->items());
                set_triggers(level_ptr->triggers());
                set_level_version(level_ptr->version());
                set_ng_plus(level_ptr->ng_plus());
            }
        }
        else if (auto ng_plus = messages::read_ng_plus(message))
        {
            set_ng_plus(ng_plus.value());
        }
        else if (message.type == "item_filters")
        {
            set_filters(std::static_pointer_cast<MessageData<std::vector<Filters::Filter>>>(message.data)->value);
        }
    }

    void ItemsWindow::initialise()
    {
        messages::get_open_level(_messaging, weak_from_this());
        messages::get_selected_room(_messaging, weak_from_this());
        messages::get_selected_item(_messaging, weak_from_this());
    }

    std::string ItemsWindow::type() const
    {
        return "Items";
    }

    std::string ItemsWindow::title() const
    {
        return _id;
    }
}
