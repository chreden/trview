#include "ItemsWindow.h"
#include <trview.common/Strings.h>
#include <trview.common/Windows/Clipboard.h>
#include "../trview_imgui.h"
#include "RowCounter.h"

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

    IItemsWindow::~IItemsWindow()
    {
    }

    ItemsWindow::ItemsWindow(const std::shared_ptr<IClipboard>& clipboard)
        : _clipboard(clipboard)
    {
        _tips["OCB"] = "Changes entity behaviour";
        _tips["Clear Body"] = "If true, removed when Bodybag is triggered";
        _tips["Trigger triggerer"] = "Disables the trigger on the same sector until this item is triggered";
        _tips["Type*"] = "Mutant Egg spawn target is missing; egg will be empty";

        setup_filters();
    }

    void ItemsWindow::set_items(const std::vector<std::weak_ptr<IItem>>& items)
    {
        _token_store.clear();
        _all_items = items;
        for (auto item : _all_items)
        {
            if (auto item_ptr = item.lock())
            {
                _token_store += item_ptr->on_selected += [this, item]() { set_selected_item(item); };
                if (item_ptr->selected())
                {
                    set_selected_item(item);
                }
            }
        }
        _triggered_by.clear();
        setup_filters();
        _force_sort = true;
        calculate_column_widths();
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
            _scroll_to_item = true;
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
            _scroll_to_item = true;
            set_local_selected_item(item);
        }
    }

    std::weak_ptr<IItem> ItemsWindow::selected_item() const
    {
        return _selected_item;
    }

    void ItemsWindow::render_items_list()
    {
        calculate_column_widths();
        if (ImGui::BeginChild(Names::item_list_panel.c_str(), ImVec2(0, 0), ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoScrollbar))
        {
            _filters.render();

            ImGui::SameLine();
            _track.render();

            ImGui::SameLine();
            bool sync_item = _sync_item;
            if (ImGui::Checkbox(Names::sync_item.c_str(), &sync_item))
            {
                set_sync_item(sync_item);
            }

            RowCounter counter{ "items", _all_items.size() };
            if (ImGui::BeginTable(Names::items_list.c_str(), 5, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY, ImVec2(0, -counter.height())))
            {
                ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, _column_sizer.size(0));
                ImGui::TableSetupColumn("Room", ImGuiTableColumnFlags_WidthFixed, _column_sizer.size(1));
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, _column_sizer.size(2));
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, _column_sizer.size(3));
                ImGui::TableSetupColumn("Hide", ImGuiTableColumnFlags_WidthFixed, _column_sizer.size(4));
                ImGui::TableSetupScrollFreeze(1, 1);
                ImGui::TableHeadersRow();

                imgui_sort_weak(_all_items,
                    {
                        [](auto&& l, auto&& r) { return l.number() < r.number(); },
                        [](auto&& l, auto&& r) { return std::tuple(item_room(l), l.number()) < std::tuple(item_room(r), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.type_id(), l.number()) < std::tuple(r.type_id(), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.type(), l.number()) < std::tuple(r.type(), r.number()); },
                        [](auto&& l, auto&& r) { return std::tuple(l.visible(), l.number()) < std::tuple(r.visible(), r.number()); }
                    }, _force_sort);

                for (const auto& item : _all_items)
                {
                    auto item_ptr = item.lock();
                    if (!item_ptr || (_track.enabled<Type::Room>() && item_ptr->room().lock() != _current_room.lock() || !_filters.match(*item_ptr)))
                    {
                        continue;
                    }

                    counter.count();
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    auto selected_item = _selected_item.lock();
                    bool selected = selected_item && selected_item == item_ptr;

                    ImGuiScroller scroller;
                    if (selected && _scroll_to_item)
                    {
                        scroller.scroll_to_item();
                        _scroll_to_item = false;
                    }

                    const bool item_is_virtual = is_virtual(*item_ptr);

                    ImGui::SetNextItemAllowOverlap();
                    if (ImGui::Selectable(std::format("{0}{1}##{0}", item_ptr->number(), item_is_virtual ? "*" : "").c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                    {
                        scroller.fix_scroll();

                        set_local_selected_item(item);
                        if (_sync_item)
                        {
                            item_ptr->on_selected();
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
                    ImGui::Text(std::to_string(item_room(item_ptr)).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(item_ptr->type_id()).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(item_ptr->type().c_str());
                    ImGui::TableNextColumn();
                    bool hidden = !item_ptr->visible();
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (ImGui::Checkbox(std::format("##hide-{}", item_ptr->number()).c_str(), &hidden))
                    {
                        item_ptr->set_visible(!hidden);
                    }
                    ImGui::PopStyleVar();
                }
                ImGui::EndTable();
                counter.render();
            }
        }
        ImGui::EndChild();
    }

    void ItemsWindow::render_item_details()
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
                        if (ImGui::IsItemHovered() && _tips.find(name) != _tips.end())
                        {
                            ImGui::BeginTooltip();
                            ImGui::Text(_tips[name].c_str());
                            ImGui::EndTooltip();
                        }
                        if (ImGui::IsItemHovered() && _tips.find(string_value) != _tips.end())
                        {
                            ImGui::BeginTooltip();
                            ImGui::Text(_tips[string_value].c_str());
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
            ImGui::Text("Triggered By");
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
                        [](auto&& l, auto&& r) { return std::tuple(trigger_type_name(l.type()), l.number()) < std::tuple(trigger_type_name(r.type()), r.number()); },
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
                        trigger_ptr->on_selected();
                    }
                    ImGui::TableNextColumn();
                    ImGui::Text(std::to_string(trigger_room(trigger_ptr)).c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text(trigger_type_name(trigger_ptr->type()).c_str());
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
        _filters.add_multi_getter<float>("Triggered By", [](auto&& item)
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
    }

    void ItemsWindow::set_level_version(trlevel::LevelVersion version)
    {
        _level_version = version;
    }

    void ItemsWindow::set_model_checker(const std::function<bool(uint32_t)>& checker)
    {
        _model_checker = checker;
    }

    void ItemsWindow::calculate_column_widths()
    {
        _column_sizer.reset();
        _column_sizer.measure("#__", 0);
        _column_sizer.measure("Room__", 1);
        _column_sizer.measure("ID__", 2);
        _column_sizer.measure("Type__", 3);
        _column_sizer.measure("Hide____", 4);

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
}
