#pragma once

#include <external/imgui/imgui.h>
#include <external/imgui/imgui_internal.h>
#include <external/imgui/misc/cpp/imgui_stdlib.h>
#include <charconv>
#include <ranges>

#include "../trview_imgui.h"

namespace trview
{
    template <typename T>
    int Filters<T>::Filter::value_count() const noexcept
    {
        switch (compare)
        {
        case CompareOp::Between:
        case CompareOp::BetweenInclusive:
            return 2;
        case CompareOp::Exists:
        case CompareOp::NotExists:
            return 0;
        }
        return 1;
    }

    template <typename T>
    bool Filters<T>::Filter::initial_state() const noexcept
    {
        switch (compare)
        {
        case CompareOp::NotExists:
            return true;
        }
        return false;
    }

    template <typename T>
    void Filters<T>::add_filter(const Filter& filter)
    {
        _filters.push_back(filter);
        _changed = true;
    }

    template <typename T>
    template <typename value_type>
    void Filters<T>::add_getter(const std::string& key, const std::function<value_type(const T&)>& getter, EditMode can_change)
    {
        add_getter(key, getter, {}, can_change);
    }

    template <typename T>
    template <typename value_type>
    void Filters<T>::add_getter(const std::string& key, const std::vector<std::string>& options, const std::function<value_type(const T&)>& getter, EditMode can_change)
    {
        add_getter(key, options, getter, {}, can_change);
    }

    template <typename T>
    template <typename value_type>
    void Filters<T>::add_getter(const std::string& key, const std::function<value_type(const T&)>& getter, const std::function<bool(const T&)>& predicate, EditMode can_change)
    {
        add_getter(key, available_options<value_type>(), getter, predicate, can_change);
    }

    template <typename T>
    template <typename value_type>
    void Filters<T>::add_getter(const std::string& key, const std::vector<std::string>& options, const std::function<value_type(const T&)>& getter, const std::function<bool(const T&)>& predicate, EditMode can_change)
    {
        _getters[key] =
        {
            .ops = compare_ops<value_type>(),
            .options = options,
            .function = [=](const auto& value) { return getter(value); },
            .predicate = predicate,
            .can_change = can_change
        };
    }

    template <typename T>
    template <typename value_type>
    void Filters<T>::add_multi_getter(const std::string& key, const std::function<std::vector<value_type>(const T&)>& getter)
    {
        add_multi_getter(key, getter, {});
    }

    template <typename T>
    template <typename value_type>
    void Filters<T>::add_multi_getter(const std::string& key, const std::vector<std::string>& options, const std::function<std::vector<value_type>(const T&)>& getter)
    {
        add_multi_getter(key, options, getter, {});
    }

    template <typename T>
    template <typename value_type>
    void Filters<T>::add_multi_getter(const std::string& key, const std::function<std::vector<value_type>(const T&)>& getter, const std::function<bool(const T&)>& predicate)
    {
        add_multi_getter(key, available_options<value_type>(), getter, predicate);
    }

    template <typename T>
    template <typename value_type>
    void Filters<T>::add_multi_getter(const std::string& key, const std::vector<std::string>& options, const std::function<std::vector<value_type>(const T&)>& getter, const std::function<bool(const T&)>& predicate)
    {
        _multi_getters[key] =
        {
            compare_ops<value_type>(),
            options,
            [=](const auto& value)
            {
                const auto results = getter(value);
                std::vector<Value> values;
                std::transform(results.begin(), results.end(), std::back_inserter(values), [](const auto& v) { return v; });
                return values;
            },
            predicate
        };
    }

    template <typename T>
    void Filters<T>::clear_all_getters()
    {
        _getters.clear();
        _multi_getters.clear();
    }

    template <typename T>
    void Filters<T>::force_sort()
    {
        _force_sort = true;
    }

    template <typename T>
    std::vector<std::string> Filters<T>::keys() const
    {
        std::vector<std::string> result;
        for (const auto& key : _getters)
        {
            result.push_back(key.first);
        }
        for (const auto& key : _multi_getters)
        {
            result.push_back(key.first);
        }
        return result;
    }

    template <typename T>
    bool Filters<T>::empty() const
    {
        return _filters.empty() || std::all_of(_filters.begin(), _filters.end(), [](auto&& f) { return f.key == "" && f.value == "" && f.children.empty(); });
    }

    template <typename T>
    bool Filters<T>::is_match(const Value& value, const Filter& filter) const
    {
        if (const std::string* value_string = std::get_if<std::string>(&value))
        {
            return is_match(*value_string, filter);
        }
        else if (const float* value_float = std::get_if<float>(&value))
        {
            return is_match(*value_float, filter);
        }
        else if (const bool* value_bool = std::get_if<bool>(&value))
        {
            return is_match(*value_bool, filter);
        }
        else if (const int* value_int = std::get_if<int>(&value))
        {
            return is_match(static_cast<float>(*value_int), filter);
        }
        return false;
    }

    template <typename T>
    bool Filters<T>::is_match(const std::string& value, const Filter& filter) const
    {
        switch (filter.compare)
        {
        case CompareOp::Equal:
            return value == filter.value;
        case CompareOp::NotEqual:
            return value != filter.value;
        case CompareOp::Exists:
            return true;
        case CompareOp::StartsWith:
            return value.starts_with(filter.value);
        case CompareOp::EndsWith:
            return value.ends_with(filter.value);
        }
        return false;
    }

    template <typename T>
    bool Filters<T>::is_match(float value, const Filter& filter) const
    {
        if (filter.compare == CompareOp::Exists)
        {
            return true;
        }

        float float_value = 0;
        auto [_, error] { std::from_chars(filter.value.data(), filter.value.data() + filter.value.size(), float_value) };
        if (error != std::errc())
        {
            return false;
        }

        float float_value2 = 0;
        auto [d_, error2] { std::from_chars(filter.value2.data(), filter.value2.data() + filter.value2.size(), float_value2) };

        switch (filter.compare)
        {
        case CompareOp::Equal:
            return value == float_value;
        case CompareOp::NotEqual:
            return value != float_value;
        case CompareOp::GreaterThan:
            return value > float_value;
        case CompareOp::GreaterThanOrEqual:
            return value >= float_value;
        case CompareOp::LessThan:
            return value < float_value;
        case CompareOp::LessThanOrEqual:
            return value <= float_value;
        case CompareOp::Between:
            return error2 == std::errc() && value > float_value && value < float_value2;
        case CompareOp::BetweenInclusive:
            return error2 == std::errc() && value >= float_value && value <= float_value2;
        }
        return false;
    }

    template <typename T>
    bool Filters<T>::is_match(bool value, const Filter& filter) const
    {
        const bool actual_value = filter.value == "true";
        switch (filter.compare)
        {
        case CompareOp::Equal:
            return value == actual_value;
        case CompareOp::NotEqual:
            return value != actual_value;
        case CompareOp::Exists:
            return true;
        }
        return false;
    }

    template <typename T>
    bool Filters<T>::group_match(std::ranges::input_range auto&& results, const Filter& filter) const
    {
        if (filter.compare == CompareOp::NotEqual)
        {
            return std::ranges::all_of(results, [](auto&& v) { return v; });
        }
        return std::ranges::any_of(results, [](auto&& v) { return v; });
    }

    template <typename T>
    bool Filters<T>::match(const Filters<T>::Filter& filter, const T& value) const
    {
        bool filter_result = filter.initial_state();

        if (!filter.children.empty())
        {
            bool child_match = false;
            Op child_op = Op::Or;

            for (const auto& child : filter.children)
            {
                const bool child_filter_result = match(child, value);

                child_match = child_op == Op::Or ? child_match | child_filter_result : child_match & child_filter_result;
                child_op = child.op;

                if (child_op == Op::And && !child_match)
                {
                    break;
                }
            }

            filter_result = child_match;
        }
        else
        {
            const auto& getter = _getters.find(filter.key);
            if (getter != _getters.end())
            {
                const auto getter_predicate = getter->second.predicate;
                if (!getter_predicate || getter_predicate(value))
                {
                    const auto getter_value = getter->second.function(value);
                    filter_result = is_match(getter_value, filter);
                }
            }
            else
            {
                const auto& multi_getter = _multi_getters.find(filter.key);
                if (multi_getter != _multi_getters.end())
                {
                    const auto getter_predicate = multi_getter->second.predicate;
                    if (!getter_predicate || getter_predicate(value))
                    {
                        const auto getter_values = multi_getter->second.function(value);
                        if (!getter_values.empty())
                        {
                            filter_result = group_match(getter_values | std::views::transform([&](auto&& value) { return is_match(value, filter); }), filter);
                        }
                    }
                }
            }
        }

        return filter_result;
    }

    template <typename T>
    bool Filters<T>::match(const T& value) const
    {
        if (!_enabled || empty())
        {
            return true;
        }

        bool match = false;
        Op op = Op::Or;
        for (const auto& filter : _filters)
        {
            const bool filter_result = this->match(filter, value); 
            match = op == Op::Or ? match | filter_result : match & filter_result;
            op = filter.op;

            if (op == Op::And && !match)
            {
                break;
            }
        }
        return match;
    }

    template <typename T>
    void Filters<T>::toggle_visible()
    {
        if (!_show_filters)
        {
            ImGui::OpenPopup(Names::Popup.c_str());
        }
        _show_filters = !_show_filters;
    }

    template <typename T>
    void Filters<T>::render()
    {
        bool filter_enabled = _enabled;
        if (ImGui::Checkbox(Names::Enable.c_str(), &filter_enabled))
        {
            _enabled = filter_enabled;
            _changed = true;
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::Text("Toggle filters");
            ImGui::EndTooltip();
        }
        ImGui::SameLine();

        if (ImGui::Button(Names::FiltersButton.c_str()))
        {
            toggle_visible();
        }

        if (_show_filters && ImGui::BeginPopup(Names::Popup.c_str()))
        {
            const auto keys = this->keys();

            ImGui::Text("Filters");

            std::vector<uint32_t> remove;
            for (uint32_t i = 0; i < _filters.size(); ++i)
            {
                auto& filter = _filters[i];
                if (ImGui::BeginCombo((Names::FilterKey + std::to_string(i)).c_str(), filter.key.c_str()))
                {
                    for (const auto& key : keys)
                    {
                        if (ImGui::Selectable(key.c_str(), key == filter.key))
                        {
                            filter.key = key;
                            _changed = true;

                            // If the current value is not in the options then set to one of them.
                            if (has_options(filter.key))
                            {
                                const auto options = options_for_key(filter.key);
                                bool value_valid = std::find(options.begin(), options.end(), filter.value) != options.end();
                                if (!value_valid)
                                {
                                    filter.value = options.back();
                                }
                            }

                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();

                auto available_compare_ops = ops_for_key(filter.key);
                if (ImGui::BeginCombo((Names::FilterCompareOp + std::to_string(i)).c_str(), to_string(filter.compare).c_str()))
                {
                    for (const auto& compare_op : available_compare_ops)
                    {
                        if (ImGui::Selectable(to_string(compare_op).c_str(), compare_op == filter.compare))
                        {
                            filter.compare = compare_op;
                            _changed = true;
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();

                if (has_options(filter.key) && filter.compare != CompareOp::StartsWith && filter.compare != CompareOp::EndsWith)
                {
                    auto available_options = options_for_key(filter.key);
                    if (filter.value_count() > 0 && ImGui::BeginCombo((Names::FilterValue + std::to_string(i)).c_str(), filter.value.c_str()))
                    {
                        for (const auto& option : available_options)
                        {
                            if (ImGui::Selectable(option.c_str(), option == filter.value))
                            {
                                filter.value = option;
                                _changed = true;
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::SameLine();

                    if (filter.value_count() > 1 && ImGui::BeginCombo((Names::FilterValue + "2-" + std::to_string(i)).c_str(), filter.value2.c_str()))
                    {
                        for (const auto& option : available_options)
                        {
                            if (ImGui::Selectable(option.c_str(), option == filter.value2))
                            {
                                filter.value2 = option;
                                _changed = true;
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::SameLine();
                }
                else
                {
                    if (filter.value_count() > 0)
                    {
                        if (ImGui::InputText((Names::FilterValue + std::to_string(i)).c_str(), &filter.value))
                        {
                            _changed = true;
                        }
                        ImGui::SameLine();
                    }
                    if (filter.value_count() > 1)
                    {
                        if (ImGui::InputText((Names::FilterValue + "2-" + std::to_string(i)).c_str(), &filter.value2))
                        {
                            _changed = true;
                        }
                        ImGui::SameLine();
                    }
                }

                if (ImGui::Button((Names::RemoveFilter + std::to_string(i)).c_str()))
                {
                    _changed = true;
                    remove.push_back(i);
                }

                if (i != _filters.size() - 1)
                {
                    std::vector<Op> ops{ Op::And, Op::Or };
                    if (ImGui::BeginCombo((Names::FilterOp + std::to_string(i)).c_str(), to_string(filter.op).c_str()))
                    {
                        for (const auto& op : ops)
                        {
                            if (ImGui::Selectable(to_string(op).c_str(), op == filter.op))
                            {
                                filter.op = op;
                                _changed = true;
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }
                }
            }

            for (auto iter = remove.rbegin(); iter < remove.rend(); ++iter)
            {
                _filters.erase(_filters.begin() + *iter);
            }

            if (ImGui::Button(Names::AddFilter.c_str()))
            {
                _filters.push_back({});
                _changed = true;
            }
            ImGui::EndPopup();
        }
        else
        {
            _show_filters = false;
        }
    }

    template <typename T>
    void Filters<T>::render_settings()
    {
        if (ImGui::Button("Columns"))
        {
            ImGui::OpenPopup("Columns");
        }

        if (ImGui::BeginPopupContextItem("Columns"))
        {
            if (ImGui::Button("Save"))
            {
                on_columns_saved();
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Save these columns as the default for this list");
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset"))
            {
                on_columns_reset();
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Reset columns for this list to default");
            }

            if (ImGui::BeginTable("Columns List", 2, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY))
            {
                ImGui::TableSetupColumn("Name");
                ImGui::TableSetupColumn("Visible");
                ImGui::TableSetupScrollFreeze(0, 1);
                for (auto& getter : _getters)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text(getter.first.c_str());
                    ImGui::TableNextColumn();
                    bool visible = std::ranges::find(_columns, getter.first) != _columns.end();
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                    if (ImGui::Checkbox(std::format("##{}-visible", getter.first).c_str(), &visible))
                    {
                        if (visible)
                        {
                            _columns.push_back(getter.first);
                        }
                        else
                        {
                            // Remove the column but also order the columns to be how they are in the
                            // current display order.
                            std::vector<std::string> new_columns;
                            for (std::size_t col = 0; col < _columns.size() && col < _column_order.size(); ++col)
                            {
                                if (_columns[_column_order[col]] != getter.first)
                                {
                                    new_columns.push_back(_columns[_column_order[col]]);
                                }
                            }
                            _columns = new_columns;
                            _column_order = std::views::iota(static_cast<std::size_t>(0u), _columns.size()) | std::ranges::to<std::vector>();
                        }
                    }
                    ImGui::PopStyleVar();
                }
                ImGui::EndTable();
            }
            ImGui::EndPopup();
        }
    }

    template <typename T>
    std::vector<CompareOp> Filters<T>::ops_for_key(const std::string& key) const
    {
        const auto& getter = _getters.find(key);
        if (getter != _getters.end())
        {
            auto ops = getter->second.ops;
            if (getter->second.predicate)
            {
                ops.push_back(CompareOp::Exists);
                ops.push_back(CompareOp::NotExists);
            }
            return ops;
        }
        else
        {
            const auto& multi_getter = _multi_getters.find(key);
            if (multi_getter != _multi_getters.end())
            {
                auto ops = multi_getter->second.ops;
                ops.push_back(CompareOp::Exists);
                ops.push_back(CompareOp::NotExists);
                return ops;
            }
        }

        return
        {
            CompareOp::Equal,
            CompareOp::NotEqual,
            CompareOp::GreaterThan,
            CompareOp::GreaterThanOrEqual,
            CompareOp::LessThan,
            CompareOp::LessThanOrEqual,
            CompareOp::Between,
            CompareOp::BetweenInclusive
        };
    }

    template <typename T>
    std::vector<std::string> Filters<T>::options_for_key(const std::string& key) const
    {
        const auto& getter = _getters.find(key);
        if (getter != _getters.end())
        {
            return getter->second.options;
        }
        else
        {
            const auto& multi_getter = _multi_getters.find(key);
            if (multi_getter != _multi_getters.end())
            {
                return multi_getter->second.options;
            }
        }
        return {};
    }

    template <typename T>
    bool Filters<T>::has_options(const std::string& key) const
    {
        const auto& getter = _getters.find(key);
        if (getter != _getters.end())
        {
            return !getter->second.options.empty();
        }
        else
        {
            const auto& multi_getter = _multi_getters.find(key);
            if (multi_getter != _multi_getters.end())
            {
                return !multi_getter->second.options.empty();
            }
        }
        return false;
    }

    template <typename T>
    void Filters<T>::scroll_to_item()
    {
        _scroll_to_item = true;
    }

    template <typename T>
    void Filters<T>::set_columns(const std::vector<std::string>& columns)
    {
        _columns = columns;
    }

    template <typename T>
    void Filters<T>::set_filters(const std::vector<Filter> filters)
    {
        _filters = filters;
    }

    template <typename T>
    bool Filters<T>::test_and_reset_changed()
    {
        bool current_value = _changed;
        _changed = false;
        return current_value;
    }

    template <typename T>
    int Filters<T>::column_count() const
    {
        return static_cast<int>(_columns.size());
    }

    template <typename T>
    std::vector<std::string> Filters<T>::columns() const
    {
        return _columns;
    }

    template <typename T>
    void Filters<T>::render_table(const std::ranges::forward_range auto& items,
        std::ranges::forward_range auto& all_items,
        const std::weak_ptr<T>& selected_item,
        RowCounter counter,
        const std::function<void(std::weak_ptr<T>)>& on_item_selected,
        const std::unordered_map<std::string, Toggle>& on_toggle)
    {
        auto columns = column_count();
        if (columns == 0)
        {
            return;
        }

        if (ImGui::BeginTable("filter-list", columns, ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Reorderable, ImVec2(0, -counter.height())))
        {
            for (const auto& column_name : _columns)
            {
                ImGui::TableSetupColumn(column_name.c_str(), ImGuiTableColumnFlags_WidthFixed);
            }
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableNextRow(ImGuiTableRowFlags_Headers, ImGui::TableGetHeaderRowHeight());

            int column_n = 0;
            for (const auto& column_name : _columns)
            {
                if (!ImGui::TableSetColumnIndex(column_n++))
                {
                    continue;
                }

                const auto found_getter = _getters.find(column_name);
                if (found_getter == _getters.end())
                {
                    continue;
                }
                const auto& getter = found_getter->second;
                if (getter.can_change == EditMode::ReadWrite)
                {
                    auto found_toggle = on_toggle.find(column_name);
                    if (found_toggle != on_toggle.end())
                    {
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                        bool value = found_toggle->second.all_toggled();
                        if (ImGui::Checkbox("##checkall", &value))
                        {
                            found_toggle->second.on_toggle_all(value);
                        }
                        ImGui::PopStyleVar();
                        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
                    }
                }
                ImGui::TableHeader(column_name.c_str());
            }

            std::vector<std::function<bool(const T&, const T&)>> sorting_functions;
            for (const auto& column : _columns)
            {
                const auto found_getter = _getters.find(column);
                if (found_getter != _getters.end())
                {
                    const auto getter = found_getter->second;
                    sorting_functions.push_back([=](const T& l, const T& r) -> bool
                    {
                        return std::tuple(getter.function(l), l.number()) < std::tuple(getter.function(r), r.number());
                    });
                }
            }

            imgui_sort_weak(all_items, sorting_functions, _force_sort);
            _force_sort = false;

            for (const auto& item : items)
            {
                counter.count();
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                auto selected_item_ptr = selected_item.lock();
                bool selected = selected_item_ptr && selected_item_ptr == item;

                ImGuiScroller scroller;
                if (selected && _scroll_to_item)
                {
                    scroller.scroll_to_item();
                    _scroll_to_item = false;
                }

                ImGui::SetNextItemAllowOverlap();
                if (ImGui::Selectable(std::format("##{0}", item->number()).c_str(), &selected, ImGuiSelectableFlags_SpanAllColumns | static_cast<int>(ImGuiSelectableFlags_SelectOnNav)))
                {
                    scroller.fix_scroll();
                    on_item_selected(item);
                    _scroll_to_item = false;
                }
                ImGui::SameLine();

                int remaining = columns;
                for (const auto& column_name : _columns)
                {
                    const auto found_getter = _getters.find(column_name);
                    if (found_getter == _getters.end())
                    {
                        continue;
                    }
                    const auto& getter = found_getter->second;
                    const auto result = getter.function(*item);

                    std::visit([&](auto&& arg)
                        {
                            using R = std::decay_t<decltype(arg)>;
                            if constexpr (std::is_same_v<R, std::string>)
                            {
                                ImGui::Text(arg.c_str());
                            }
                            else if constexpr (std::is_same_v<R, bool>)
                            {
                                bool toggle_value = arg;

                                ImGui::BeginDisabled(getter.can_change == EditMode::Read);
                                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                                if (ImGui::Checkbox(std::format("##{}-{}", column_name, item->number()).c_str(), &toggle_value))
                                {
                                    auto found_toggle = on_toggle.find(column_name);
                                    if (found_toggle != on_toggle.end())
                                    {
                                        found_toggle->second.on_toggle(item, toggle_value);
                                    }
                                }
                                ImGui::PopStyleVar();
                                ImGui::EndDisabled();
                            }
                            else
                            {
                                ImGui::Text(std::to_string(arg).c_str());
                            }
                        }, result);

                    if (--remaining > 0)
                    {
                        ImGui::TableNextColumn();
                    }
                }
            }

            // Track current order in case a column is removed - have to restore the order
            // manually otherwise it resets.
            _column_order = { std::from_range, ImGui::GetCurrentTable()->DisplayOrderToIndex };

            ImGui::EndTable();
            counter.render();
        }
    }

    constexpr std::string to_string(CompareOp op) noexcept
    {
        switch (op)
        {
        case CompareOp::Equal:
            return "is";
        case CompareOp::NotEqual:
            return "is not";
        case CompareOp::GreaterThan:
            return "greater than";
        case CompareOp::GreaterThanOrEqual:
            return "greater than or equal";
        case CompareOp::LessThan:
            return "less than";
        case CompareOp::LessThanOrEqual:
            return "less than or equal";
        case CompareOp::Between:
            return "between";
        case CompareOp::BetweenInclusive:
            return "between inclusive";
        case CompareOp::Exists:
            return "is present";
        case CompareOp::NotExists:
            return "is not present";
        case CompareOp::StartsWith:
            return "starts with";
        case CompareOp::EndsWith:
            return "ends with";
        }
        return "?";
    }

    constexpr std::string to_string(Op op) noexcept
    {
        switch (op)
        {
        case Op::And:
            return "And";
        case Op::Or:
            return "Or";
        }
        return "?";
    }

    template <typename T>
    constexpr std::vector<CompareOp> compare_ops() noexcept
    {
        return
        {
            CompareOp::Equal,
            CompareOp::NotEqual,
            CompareOp::StartsWith,
            CompareOp::EndsWith
        };
    }

    template <>
    constexpr std::vector<CompareOp> compare_ops<float>() noexcept
    {
        return
        {
            CompareOp::Equal,
            CompareOp::NotEqual,
            CompareOp::GreaterThan,
            CompareOp::GreaterThanOrEqual,
            CompareOp::LessThan,
            CompareOp::LessThanOrEqual,
            CompareOp::Between,
            CompareOp::BetweenInclusive
        };
    }

    template <>
    constexpr std::vector<CompareOp> compare_ops<bool>() noexcept
    {
        return
        {
            CompareOp::Equal,
            CompareOp::NotEqual,
        };
    }

    template <>
    constexpr std::vector<CompareOp> compare_ops<int>() noexcept
    {
        return
        {
            CompareOp::Equal,
            CompareOp::NotEqual,
            CompareOp::GreaterThan,
            CompareOp::GreaterThanOrEqual,
            CompareOp::LessThan,
            CompareOp::LessThanOrEqual,
            CompareOp::Between,
            CompareOp::BetweenInclusive
        };
    }

    template <typename T>
    constexpr std::vector<std::string> available_options() noexcept
    {
        return {};
    }

    template <>
    constexpr std::vector<std::string> available_options<bool>() noexcept
    {
        return { "false", "true" };
    }

    template <typename T>
    std::unordered_map<std::string, typename Filters<T>::Toggle> default_hide(const std::vector<std::shared_ptr<T>>& filtered_entries)
    {
        return
        {
            {
                "Hide",
                {
                    .on_toggle = [&](auto&& entry, auto&& value)
                    {
                        if (auto entry_ptr = entry.lock())
                        {
                            entry_ptr->set_visible(!value);
                        }
                    },
                    .on_toggle_all = [&](bool value)
                    {
                        std::ranges::for_each(filtered_entries, [=](auto&& entry) { entry->set_visible(!value); });
                    },
                    .all_toggled = [&]() { return std::ranges::all_of(filtered_entries, [](auto&& entry) { return !entry->visible(); }); }
                }
            }
        };
    }

    template <typename T>
    std::vector<typename Filters<T>::Filter> Filters<T>::filters() const
    {
        return _filters;
    }
}
