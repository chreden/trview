#include "ItemsWindow.h"
#include <trview.common/Strings.h>
#include <trview.common/Windows/Clipboard.h>
#include "../trview_imgui.h"
#include "RowCounter.h"
#include "../Elements/ILevel.h"

namespace trview
{
    namespace
    {
        std::unordered_map<std::string, std::string> tips
        {
            { "OCB", "Changes entity behaviour" },
            { "Clear Body", "If true, removed when Bodybag is triggered" },
            { "Trigger triggerer", "Disables the trigger on the same sector until this item is triggered" },
            { "Type*", "Mutant Egg spawn target is missing; egg will be empty" }
        };

        bool is_level_mismatch(auto&& e, auto&& level)
        {
            if (auto e_ptr = e.lock())
            {
                return e_ptr->level().lock() != level.lock();
            }
            return false;
        }
    }

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

    IItemsWindow::~IItemsWindow()
    {
    }

    ItemsWindow::ItemsWindow(const std::shared_ptr<IClipboard>& clipboard)
        : _clipboard(clipboard)
    {
        
    }

    void ItemsWindow::SubWindow::set_items(const std::vector<std::weak_ptr<IItem>>& items)
    {
        _all_items = items;
        _triggered_by.clear();
        setup_filters();
        _force_sort = true;
        calculate_column_widths();
    }

    void ItemsWindow::SubWindow::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _all_triggers = triggers;
    }

    void ItemsWindow::set_current_room(const std::weak_ptr<IRoom>& room)
    {
        for (auto& sub_window : _sub_windows)
        {
            sub_window.set_current_room(room);
        }
    }

    void ItemsWindow::SubWindow::set_current_room(const std::weak_ptr<IRoom>& room)
    {
        if (is_level_mismatch(room, _level))
        {
            return;
        }

        _current_room = room;
    }

    void ItemsWindow::SubWindow::set_sync_item(bool value)
    {
        if (_sync_item != value)
        {
            _sync_item = value;
            _scroll_to_item = true;
            if (_sync_item && _global_selected_item.lock())
            {
                set_selected_item(_global_selected_item);
            }
        }
    }

    void ItemsWindow::set_selected_item(const std::weak_ptr<IItem>& item)
    {
        for (auto& window : _sub_windows)
        {
            window.set_selected_item(item);
        }
    }

    void ItemsWindow::SubWindow::set_selected_item(const std::weak_ptr<IItem>& item)
    {
        if (is_level_mismatch(item, _level))
        {
            return;
        }

        _global_selected_item = item;
        if (_sync_item)
        {
            _scroll_to_item = true;
            set_local_selected_item(item);
        }
    }

    void ItemsWindow::SubWindow::render_items_list()
    {
        calculate_column_widths();
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

            if (_auto_hider.apply(_all_items, filtered_items, _filters))
            {
                on_scene_changed();
            }

            RowCounter counter{ "items",
                static_cast<std::size_t>(std::ranges::count_if(_all_items, [this](auto&& item)
                    {
                        const auto item_ptr = item.lock();
                        return item_ptr && item_ptr->ng_plus().value_or(_ng_plus) == _ng_plus;
                    }))};
            if (ImGui::BeginTable(Names::items_list.c_str(), 5, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY, ImVec2(0, -counter.height())))
            {
                imgui_header_row(
                    {
                        { "#", _column_sizer.size(0) },
                        { "Room", _column_sizer.size(1) },
                        { "ID", _column_sizer.size(2) },
                        { "Type", _column_sizer.size(3) },
                        { .name = "Hide", .width = _column_sizer.size(4), .set_checked = [&](bool v)
                            { 
                                std::ranges::for_each(filtered_items, [=](auto&& item) { item->set_visible(!v); });
                                on_scene_changed();
                            }, .checked = std::ranges::all_of(filtered_items, [](auto&& item) { return !item->visible(); })
                        }
                    });

                imgui_sort_weak(_all_items,
                    {
                        [](auto&& l, auto&& r) { return l.number() < r.number(); },
                        [](auto&& l, auto&& r) { return std::tuple(item_room(l), l.number()) < std::tuple(item_room(r), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.type_id(), l.number()) < std::tuple(r.type_id(), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.type(), l.number()) < std::tuple(r.type(), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.visible(), l.number()) < std::tuple(r.visible(), r.number()); }
                    }, _force_sort);

                for (const auto& item : filtered_items)
                {
                    counter.count();
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    auto selected_item = _selected_item.lock();
                    bool selected = selected_item && selected_item == item;

                    ImGuiScroller scroller;
                    if (selected && _scroll_to_item)
                    {
                        scroller.scroll_to_item();
                        _scroll_to_item = false;
                    }

                    const bool item_is_virtual = is_virtual(*item);

                    ImGui::SetNextItemAllowOverlap();
                    if (ImGui::Selectable(std::format("{0}{1}##{0}", item->number(), item_is_virtual ? "*" : "").c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                    {
                        scroller.fix_scroll();

                        set_local_selected_item(item);
                        if (_sync_item)
                        {
                            on_item_selected(item);
                        }
                        _scroll_to_item = false;
                    }

                    if (item_is_virtual && ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::Text("Virtual item generated by trview");
                        ImGui::EndTooltip();
                    }

                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(item_room(item)).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(item->type_id()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(item->type().c_str());
                    ImGui::TableNextColumn();
                    bool hidden = !item->visible();
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (ImGui::Checkbox(std::format("##hide-{}", item->number()).c_str(), &hidden))
                    {
                        item->set_visible(!hidden);
                        on_scene_changed();
                    }
                    ImGui::PopStyleVar();
                }
                ImGui::EndTable();
                counter.render();
            }
        }
        ImGui::EndChild();
    }

    void ItemsWindow::SubWindow::render_item_details()
    {
        if (ImGui::BeginChild(Names::details_panel.c_str(), ImVec2(), true))
        {
            ImGui::Text("Item Details");
            if (ImGui::BeginTable(Names::item_stats.c_str(), 2, 0, ImVec2(-1, 150)))
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
                        if (ImGui::IsItemHovered() && tips.find(name) != tips.end())
                        {
                            ImGui::BeginTooltip();
                            ImGui::Text(tips[name].c_str());
                            ImGui::EndTooltip();
                        }
                        if (ImGui::IsItemHovered() && tips.find(string_value) != tips.end())
                        {
                            ImGui::BeginTooltip();
                            ImGui::Text(tips[string_value].c_str());
                            ImGui::EndTooltip();
                        }

                        ImGui::TableNextColumn();
                        ImGui::Text(string_value.c_str());
                    };

                    auto position_text = [&item]()
                    {
                        const auto pos = item->position() * trlevel::Scale;
                        return std::format("{:.0f}, {:.0f}, {:.0f}", pos.x, pos.y, pos.z);
                    };

                    auto is_bad_mutant_egg = [&]() 
                    { 
                        return _level_version == trlevel::LevelVersion::Tomb1 &&
                            is_mutant_egg(*item) &&
                            !_model_checker(mutant_egg_contents(*item));
                    };

                    add_stat(std::format("Type{}", is_bad_mutant_egg() ? "*" : ""), item->type());
                    add_stat("#", item->number());
                    add_stat("Position", position_text());
                    add_stat("Angle", bound_rotation(item->angle()));
                    add_stat("Angle Degrees", bound_rotation(item->angle()) / 182);
                    add_stat("Type ID", item->type_id());
                    add_stat("Room", item_room(item));
                    add_stat("Clear Body", item->clear_body_flag());
                    add_stat("Invisible", item->invisible_flag());
                    add_stat("Flags", format_binary(item->activation_flags()));
                    add_stat("OCB", item->ocb());
                    add_stat("Category", join(item->categories()));
                }

                ImGui::EndTable();
            }
            if (ImGui::Button(Names::add_to_route_button.c_str(), ImVec2(-1, 30)))
            {
                on_add_to_route(_selected_item);
            }
            ImGui::Text("Trigger References");
            if (ImGui::BeginTable(Names::triggers_list.c_str(), 3, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable | ImGuiTableFlags_SizingFixedFit, ImVec2(-1, -1)))
            {
                ImGui::TableSetupColumn("#");
                ImGui::TableSetupColumn("Room");
                ImGui::TableSetupColumn("Type");
                ImGui::TableSetupScrollFreeze(1, 1);
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
                        on_trigger_selected(trigger);
                    }
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(trigger_room(trigger_ptr)).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(to_string(trigger_ptr->type()).c_str());
                }

                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }

    bool ItemsWindow::render_items_window()
    {
        bool stay_open = true;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(540, 500));
        if (ImGui::Begin(_id.c_str(), &stay_open))
        {
            if (ImGui::BeginTabBar("TabBar"))
            {
                int window_index = 0;
                for (auto& sub_window : _sub_windows)
                {
                    sub_window.render(window_index++);
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        return stay_open;
    }

    void ItemsWindow::add_level(const std::weak_ptr<ILevel>& level)
    {
        if (const auto new_level = level.lock())
        {
            _sub_windows.push_back(
                {
                    ._clipboard = _clipboard,
                    ._level = level,
                    ._level_version = new_level->version(),
                    ._model_checker = [=](uint32_t id)
                        { 
                            auto new_level = level.lock();
                            return new_level ? new_level->has_model(id) : false;
                        }
                });

            auto& new_window = _sub_windows.back();
            new_window.setup_filters();
            new_window.set_items(new_level->items());
            new_window.set_triggers(new_level->triggers());

            new_window.on_item_selected += on_item_selected;
            new_window.on_add_to_route += on_add_to_route;
            new_window.on_scene_changed += on_scene_changed;
            new_window.on_trigger_selected += on_trigger_selected;
        }
    }

    void ItemsWindow::set_filters(const std::weak_ptr<ILevel>& level, std::vector<Filters<IItem>::Filter> filters)
    {
        for (auto& sub_window : _sub_windows)
        {
            sub_window.set_filters(level, filters);
        }
    }

    void ItemsWindow::SubWindow::set_filters(const std::weak_ptr<ILevel>& level, std::vector<Filters<IItem>::Filter> filters)
    {
        if (level.lock() != _level.lock())
        {
            return;
        }
        _filters.set_filters(filters);
    }

    void ItemsWindow::render()
    {
        if (!render_items_window())
        {
            on_window_closed();
            return;
        }
    }

    void ItemsWindow::update(float delta)
    {
        for (auto& sub_window : _sub_windows)
        {
            sub_window.update(delta);
        }
    }

    void ItemsWindow::SubWindow::update(float delta)
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

    void ItemsWindow::SubWindow::set_local_selected_item(std::weak_ptr<IItem> item)
    {
        _selected_item = item;
        if (auto selected = _selected_item.lock())
        {
            _triggered_by = selected->triggers();
        }
        _force_sort = true;
    }

    void ItemsWindow::SubWindow::setup_filters()
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
        _filters.add_getter<std::string>("Type", { available_types.begin(), available_types.end() }, [](auto&& item) { return item.type(); });
        _filters.add_multi_getter<std::string>("Category", { available_categories.begin(), available_categories.end() }, [](auto&& item)
            {
                std::vector<std::string> results;
                for (const auto& category : item.categories())
                {
                    results.push_back(category);
                }
                return results;
            });
        _filters.add_getter<float>("#", [](auto&& item) { return static_cast<float>(item.number()); });
        _filters.add_getter<float>("X", [](auto&& item) { return item.position().x * trlevel::Scale_X; });
        _filters.add_getter<float>("Y", [](auto&& item) { return item.position().y * trlevel::Scale_Y; });
        _filters.add_getter<float>("Z", [](auto&& item) { return item.position().z * trlevel::Scale_Z; });
        _filters.add_getter<float>("Angle", [](auto&& item) { return static_cast<float>(bound_rotation(item.angle())); });
        _filters.add_getter<float>("Angle Degrees", [](auto&& item) { return static_cast<float>(bound_rotation(item.angle()) / 182); });
        _filters.add_getter<float>("Type ID", [](auto&& item) { return static_cast<float>(item.type_id()); });
        _filters.add_getter<float>("Room", [](auto&& item) { return static_cast<float>(item_room(item)); });
        _filters.add_getter<bool>("Clear Body", [](auto&& item) { return item.clear_body_flag(); });
        _filters.add_getter<bool>("Invisible", [](auto&& item) { return item.invisible_flag(); });
        _filters.add_getter<std::string>("Flags", [](auto&& item) { return format_binary(item.activation_flags()); });
        _filters.add_getter<float>("OCB", [](auto&& item) { return static_cast<float>(item.ocb()); });
        _filters.add_multi_getter<float>("Trigger References", [](auto&& item)
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
            });
        _filters.add_multi_getter<bool>("NG+", [](auto&& item)
            {
                return item.ng_plus() == std::nullopt ? std::vector<bool>{} : std::vector<bool>{false,true};
            });
    }

    void ItemsWindow::SubWindow::calculate_column_widths()
    {
        _column_sizer.reset();
        _column_sizer.measure("#__", 0);
        _column_sizer.measure("Room__", 1);
        _column_sizer.measure("ID__", 2);
        _column_sizer.measure("Type__", 3);
        _column_sizer.measure("Hide________", 4);

        for (const auto& item : _all_items)
        {
            if (auto item_ptr = item.lock())
            {
                const bool item_is_virtual = is_virtual(*item_ptr);
                _column_sizer.measure(std::format("{0}{1}##{0}", item_ptr->number(), item_is_virtual ? "*" : ""), 0);
                _column_sizer.measure(std::to_string(item_room(item_ptr)), 1);
                _column_sizer.measure(std::to_string(item_ptr->type_id()), 2);
                _column_sizer.measure(item_ptr->type(), 3);
            }
        }
    }

    std::string ItemsWindow::name() const
    {
        return _id;
    }

    void ItemsWindow::SubWindow::render(int index)
    {
        if (const auto& level = _level.lock())
        {
            if (ImGui::BeginTabItem(std::format("{}##{}", level->name(), index).c_str()))
            {
                _ng_plus = level->ng_plus();

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

                ImGui::EndTabItem();
            }
        }
    }
}
