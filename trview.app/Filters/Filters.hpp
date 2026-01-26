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
        _filter.children.push_back(filter);
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
        return _filter.children.empty()
            || std::ranges::all_of(_filter.children, [](auto&& c) { return c.key == "" && c.value == "" && c.children.empty(); });
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

        return filter_result ^ filter.invert;
    }

    template <typename T>
    bool Filters<T>::match(const T& value) const
    {
        return _filter.children.empty() || match(_filter, value);
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
    Filters<T>::Action Filters<T>::render(Filter& filter, const std::vector<std::string>& keys, int32_t depth, int32_t index, Filter& parent)
    {
        // For the 0th element we always just draw children.
        if (!filter.children.empty() || depth == 0)
        {
            const std::string suffix = std::format("{}-{}", depth, index);

            if (ImGui::BeginChild((std::string("FilterGroup##") + suffix).c_str(), ImVec2(), ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY))
            {
                int32_t child_index = 0;
                for (auto& child : filter.children)
                {
                    const std::string child_suffix = std::format("{}-{}-{}", depth, index, child_index);
                    if (Action::Remove == render(child, keys, depth + 1, child_index, filter))
                    {
                        filter.children.erase(filter.children.begin() + child_index);
                        break;
                    }

                    if (depth > 0)
                    {
                        ImGui::SameLine();
                        if (ImGui::Button((std::string("<##") + child_suffix).c_str()))
                        {
                            const auto filter_to_promote = child;
                            filter.children.erase(filter.children.begin() + child_index);

                            const auto filter_in_parent = std::ranges::find(parent.children, filter);
                            const auto filter_in_parent_index = filter_in_parent - parent.children.begin();

                            const bool last_filter_in_filter = filter.children.size() == 0;
                            parent.children.insert(filter_in_parent, filter_to_promote);

                            if (last_filter_in_filter)
                            {
                                parent.children.erase(parent.children.begin() + filter_in_parent_index + 1);
                            }

                            break;
                        }
                        if (ImGui::IsItemHovered())
                        {
                            ImGui::SetTooltip("Move this condition into the parent condition");
                        }
                    }

                    ImGui::SameLine();
                    if (ImGui::Button((std::string(">##") + child_suffix).c_str()))
                    {
                        auto filter_to_group = child;
                        child = {};
                        child.children.push_back(filter_to_group);
                        break;
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Make this condition a child of the current condition");
                    }

                    if (child_index != filter.children.size() - 1)
                    {
                        std::vector<Op> ops{ Op::And, Op::Or };
                        if (ImGui::BeginCombo((Names::FilterOp + child_suffix).c_str(), to_string(child.op).c_str()))
                        {
                            for (const auto& op : ops)
                            {
                                if (ImGui::Selectable(to_string(op).c_str(), op == child.op))
                                {
                                    child.op = op;
                                    _changed = true;
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }
                    }
                    ++child_index;
                }

                if (ImGui::Button(std::format("{}##{}", Names::AddFilter, suffix).c_str()))
                {
                    _changed = true;
                    filter.children.push_back({});
                }
            }

            ImGui::EndChild();
        }
        else
        {
            const std::string suffix = std::format("{}-{}", depth, index);

            const std::string not_id = "!##" + suffix;
            const bool inverted = filter.invert;
            if (inverted)
            {
                ImGui::PushID(not_id.c_str());
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.06f, 0.53f, 0.98f, 1.00f));
                ImGui::PopID();
            }
            if (ImGui::Button(not_id.c_str()))
            {
                _changed = true;
                filter.invert = !filter.invert;
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Invert the condition");
            }
            if (inverted)
            {
                ImGui::PopStyleColor();
            }
            ImGui::SameLine();

            if (ImGui::BeginCombo((Names::FilterKey + suffix).c_str(), filter.key.c_str()))
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
            if (ImGui::BeginCombo((Names::FilterCompareOp + suffix).c_str(), to_string(filter.compare).c_str()))
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
                if (filter.value_count() > 0 && ImGui::BeginCombo((Names::FilterValue + suffix).c_str(), filter.value.c_str()))
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

                if (filter.value_count() > 1 && ImGui::BeginCombo((Names::FilterValue + "2-" + suffix).c_str(), filter.value2.c_str()))
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
                    if (ImGui::InputText((Names::FilterValue + suffix).c_str(), &filter.value))
                    {
                        _changed = true;
                    }
                    ImGui::SameLine();
                }
                if (filter.value_count() > 1)
                {
                    if (ImGui::InputText((Names::FilterValue + "2-" + suffix).c_str(), &filter.value2))
                    {
                        _changed = true;
                    }
                    ImGui::SameLine();
                }
            }

            if (ImGui::Button((Names::RemoveFilter + suffix).c_str()))
            {
                _changed = true;
                return Action::Remove;
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Remove this condition");
            }
        }

        return Action::None;
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
            render(_filter, keys, 0, 0, _filter);
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
        _filter.children = filters;
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
        case CompareOp::Matches:
            return "matches";
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

    template <>
    constexpr std::vector<CompareOp> compare_ops<std::weak_ptr<IFilterable>>() noexcept
    {
        return
        {
            CompareOp::Matches
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
        return _filter.children;
    }

    template <typename T>
    Filters2::Getters Filters2::GettersBuilder::build() const
    {
        return
        {
            .type_key = _type_key,
            .type_matcher = [](const std::weak_ptr<IFilterable>& f)
            {
                const auto f_ptr = f.lock();
                return f_ptr && std::dynamic_pointer_cast<T>(f_ptr);
            },
            .getters = _getters,
            .multi_getters = _multi_getters
        };
    }

    template <typename T, typename ValueType>
    Filters2::GettersBuilder& Filters2::GettersBuilder::with_getter(const std::string& key, const std::function<ValueType (const T&)>& getter, EditMode can_change)
    {
        return with_getter(key, available_options<ValueType>(), getter, {}, can_change, "");
    }

    template <typename T, typename ValueType>
    Filters2::GettersBuilder& Filters2::GettersBuilder::with_getter(const std::string& key, const std::vector<std::string>& options, const std::function<ValueType(const T&)>& getter, EditMode can_change)
    {
        return with_getter(key, options, getter, {}, can_change, "");
    }

    template <typename T, typename ValueType>
    Filters2::GettersBuilder& Filters2::GettersBuilder::with_getter(const std::string& key, const std::function<ValueType(const T&)>& getter, const std::function<bool(const T&)>& predicate, EditMode can_change)
    {
        return with_getter(key, available_options<ValueType>(), getter, predicate, can_change, "");
    }

    template <typename T, typename ValueType>
    Filters2::GettersBuilder& Filters2::GettersBuilder::with_getter(const std::string& key, const std::vector<std::string>& options, const std::function<ValueType(const T&)>& getter, const std::function<bool(const T&)>& predicate, EditMode can_change)
    {
        return with_getter(key, options, getter, predicate, can_change, "");
    }

    template <typename T, typename ValueType>
    Filters2::GettersBuilder& Filters2::GettersBuilder::with_getter(const std::string& key, const std::vector<std::string>& options, const std::function<ValueType(const T&)>& getter, const std::function<bool(const T&)>& predicate, EditMode can_change, const std::string& type_key)
    {
        ValueGetter new_getter
        {
            .ops = compare_ops<ValueType>(),
            .options = options,
            .function = [=](const IFilterable& f) -> Filters2::Value
            {
                return getter(static_cast<const T&>(f));
            },
            .can_change = can_change,
            .type_key = type_key
        };

        if (predicate)
        {
            new_getter.predicate = [=](const IFilterable& f) -> bool
                {
                    return predicate(static_cast<const T&>(f));
                };
        }

        _getters[key] = new_getter;
        return *this;
    }

    template <typename T, typename ValueType>
    Filters2::GettersBuilder& Filters2::GettersBuilder::with_multi_getter(const std::string& key, const std::function<std::vector<ValueType>(const T&)>& getter)
    {
        return with_multi_getter(key, available_options<ValueType>(), getter, {});
    }

    template <typename T, typename ValueType>
    Filters2::GettersBuilder& Filters2::GettersBuilder::with_multi_getter(const std::string& key, const std::vector<std::string>& options, const std::function<std::vector<ValueType>(const T&)>& getter)
    {
        return with_multi_getter(key, options, getter, {});
    }

    template <typename T, typename ValueType>
    Filters2::GettersBuilder& Filters2::GettersBuilder::with_multi_getter(const std::string& key, const std::function<std::vector<ValueType>(const T&)>& getter, const std::function<bool(const T&)>& predicate)
    {
        return with_multi_getter(key, available_options<ValueType>(), getter, predicate);
    }

    template <typename T, typename ValueType>
    Filters2::GettersBuilder& Filters2::GettersBuilder::with_multi_getter(const std::string& key, const std::vector<std::string>& options, const std::function<std::vector<ValueType>(const T&)>& getter, const std::function<bool(const T&)>& predicate)
    {
        return with_multi_getter(key, options, getter, predicate, "");
    }

    template <typename T, typename ValueType>
    Filters2::GettersBuilder& Filters2::GettersBuilder::with_multi_getter(const std::string& key, const std::vector<std::string>& options, const std::function<std::vector<ValueType>(const T&)>& getter, const std::function<bool(const T&)>& predicate, const std::string& type_key)
    {
        MultiGetter new_getter
        {
            .ops = compare_ops<ValueType>(),
            .options = options,
            .function = [=](const IFilterable& f) -> std::vector<Filters2::Value>
            {
                return getter(static_cast<const T&>(f)) |
                    std::ranges::to<std::vector<Filters2::Value>>();
            },
            .can_change = EditMode::Read,
            .type_key = type_key
        };

        if (predicate)
        {
            new_getter.predicate = [=](const IFilterable& f) -> bool
                {
                    return predicate(static_cast<const T&>(f));
                };
        }

        _multi_getters[key] = new_getter;
        return *this;
    }

    template <typename T>
    std::unordered_map<std::string, Filters2::Toggle> default_hide2(const std::vector<std::shared_ptr<T>>& filtered_entries)
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
                            auto t_ptr = std::static_pointer_cast<T>(entry_ptr);
                            t_ptr->set_visible(!value);
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

    template < typename T >
    void imgui_sort_weak_filters(std::vector<std::weak_ptr<T>>& container, std::vector<std::function<bool(const IFilterable&, const IFilterable&)>> callbacks, bool force_sort = false)
    {
        container.erase(std::remove_if(container.begin(), container.end(), [](auto& e) { return e.lock() == nullptr; }), container.end());

        auto specs = ImGui::TableGetSortSpecs();
        if (specs && (specs->SpecsDirty || force_sort))
        {
            std::sort(container.begin(), container.end(),
                [&](const auto& l, const auto& r) -> bool
                {
                    const auto& callback = callbacks[specs->Specs[0].ColumnIndex];
                    const auto result = callback(*l.lock(), *r.lock());
                    return specs->Specs->SortDirection == ImGuiSortDirection_Ascending ? result : !result;
                });
            specs->SpecsDirty = false;
        }
    }

    void Filters2::render_table(const std::ranges::forward_range auto& items,
        std::ranges::forward_range auto& all_items,
        const std::weak_ptr<IFilterable>& selected_item,
        RowCounter counter,
        const std::function<void(std::weak_ptr<IFilterable>)>& on_item_selected,
        const std::unordered_map<std::string, Toggle>& on_toggle)
    {
        auto columns = column_count();
        if (columns == 0)
        {
            return;
        }

        const auto& getters = find_getter(_filter.type_key);
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

                const auto found_getter = getters.getters.find(column_name);
                if (found_getter == getters.getters.end())
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

            std::vector<std::function<bool(const IFilterable&, const IFilterable&)>> sorting_functions;
            for (const auto& column : _columns)
            {
                const auto found_getter = getters.getters.find(column);
                if (found_getter != getters.getters.end())
                {
                    const auto getter = found_getter->second;
                    sorting_functions.push_back([=](const IFilterable& l, const IFilterable& r) -> bool
                        {
                            const auto l_value = getter.function(l);
                            const auto r_value = getter.function(r);
                            if (std::holds_alternative<std::string>(l_value))
                            {
                                return std::tuple(std::get<std::string>(l_value), l.filterable_index()) <
                                    std::tuple(std::get<std::string>(r_value), r.filterable_index());
                            }
                            else if (std::holds_alternative<bool>(l_value))
                            {
                                return std::tuple(std::get<bool>(l_value), l.filterable_index()) <
                                    std::tuple(std::get<bool>(r_value), r.filterable_index());
                            }
                            else if (std::holds_alternative<float>(l_value))
                            {
                                return std::tuple(std::get<float>(l_value), l.filterable_index()) <
                                    std::tuple(std::get<float>(r_value), r.filterable_index());
                            }
                            else if (std::holds_alternative<int>(l_value))
                            {
                                return std::tuple(std::get<int>(l_value), l.filterable_index()) <
                                    std::tuple(std::get<int>(r_value), r.filterable_index());
                            }
                            return l.filterable_index() < r.filterable_index();
                        });
                }
            }

            imgui_sort_weak_filters(all_items, sorting_functions, _force_sort);
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
                    const auto found_getter = getters.getters.find(column_name);
                    if (found_getter == getters.getters.end())
                    {
                        continue;
                    }
                    const auto& getter = found_getter->second;
                    const auto result = getter.function(*item);
                    result;

                    if (std::holds_alternative<std::string>(result))
                    {
                        ImGui::Text(std::get<std::string>(result).c_str());
                    }
                    else if (std::holds_alternative<bool>(result))
                    {
                        bool toggle_value = std::get<bool>(result);

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
                    else if (std::holds_alternative<float>(result))
                    {
                        ImGui::Text(std::to_string(std::get<float>(result)).c_str());
                    }
                    else if (std::holds_alternative<int>(result))
                    {
                        ImGui::Text(std::to_string(std::get<int>(result)).c_str());
                    }
                    // else if (std::holds_alternative<std::weak_ptr<IFilterable>>(result))
                    // {
                    //     // TODO: No idea what to show for this as a column.
                    // }

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
}
