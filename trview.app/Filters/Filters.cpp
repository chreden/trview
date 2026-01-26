#include "Filters.h"

namespace trview
{
    IFilterable::~IFilterable()
    {
    }

    int Filters2::Filter::value_count() const noexcept
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

    bool Filters2::Filter::initial_state() const noexcept
    {
        switch (compare)
        {
        case CompareOp::NotExists:
            return true;
        }
        return false;
    }

    Filters2::GettersBuilder& Filters2::GettersBuilder::with_type_key(const std::string& key)
    {
        _type_key = key;
        return *this;
    }

    void Filters2::add_getters(const Getters& getters)
    {
        _getters.push_back(getters);
    }

    int Filters2::column_count() const
    {
        return static_cast<int>(_columns.size());
    }

    const Filters2::Getters& Filters2::find_getter(const std::string& type_key) const
    {
        for (const auto& getter : _getters)
        {
            if (getter.type_key == type_key)
            {
                return getter;
            }
        }
        throw std::exception("Invalid type for filter");
    }

    void Filters2::clear_all_getters()
    {
        _getters.clear();
    }

    std::vector<std::string> Filters2::columns() const
    {
        return _columns;
    }

    void Filters2::force_sort()
    {
        _force_sort = true;
    }

    bool Filters2::group_match(std::ranges::input_range auto&& results, const Filter& filter) const
    {
        if (filter.compare == CompareOp::NotEqual)
        {
            return std::ranges::all_of(results, [](auto&& v) { return v; });
        }
        return std::ranges::any_of(results, [](auto&& v) { return v; });
    }

    bool Filters2::has_options(const std::string& type_key, const std::string& key) const
    {
        const auto& getters = find_getter(type_key);
        const auto& getter = getters.getters.find(key);
        if (getter != getters.getters.end())
        {
            return !getter->second.options.empty();
        }
        else
        {
            const auto& multi_getter = getters.multi_getters.find(key);
            if (multi_getter != getters.multi_getters.end())
            {
                return !multi_getter->second.options.empty();
            }
        }
        return false;
    }

    bool Filters2::is_match(const Value& value, const Filter& filter) const
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

    bool Filters2::is_match(const std::string& value, const Filter& filter) const
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

    bool Filters2::is_match(float value, const Filter& filter) const
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

    bool Filters2::is_match(bool value, const Filter& filter) const
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

    bool Filters2::match(const IFilterable& value) const
    {
        return _filter.children.empty() || match(_filter, value, _filter.type_key);
    }

    bool Filters2::match(const Filters2::Filter& filter, const IFilterable& value, const std::string& type_key) const
    {
        const std::string current_type_key = filter.type_key != "" ? filter.type_key : type_key;
        bool filter_result = filter.initial_state();

        if (!filter.children.empty())
        {
            bool child_match = false;
            Op child_op = Op::Or;

            for (const auto& child : filter.children)
            {
                const bool child_filter_result = match(child, value, current_type_key);

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
            const auto& getters = find_getter(current_type_key);
            const auto& getter = getters.getters.find(filter.key);
            if (getter != getters.getters.end())
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
                const auto& multi_getter = getters.multi_getters.find(current_type_key);
                if (multi_getter != getters.multi_getters.end())
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

    std::vector<CompareOp> Filters2::ops_for_key(const std::string& type_key, const std::string& key) const
    {
        const auto& getters = find_getter(type_key);
        const auto& getter = getters.getters.find(key);
        if (getter != getters.getters.end())
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
            const auto& multi_getter = getters.multi_getters.find(key);
            if (multi_getter != getters.multi_getters.end())
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

    std::vector<std::string> Filters2::options_for_key(const std::string& type_key, const std::string& key) const
    {
        const auto& getters = find_getter(type_key);
        const auto& getter = getters.getters.find(key);
        if (getter != getters.getters.end())
        {
            return getter->second.options;
        }
        else
        {
            const auto& multi_getter = getters.multi_getters.find(key);
            if (multi_getter != getters.multi_getters.end())
            {
                return multi_getter->second.options;
            }
        }
        return {};
    }

    std::vector<std::string> Filters2::keys(const std::string& type_key) const
    {
        std::vector<std::string> result;
        const auto& getters = find_getter(type_key);
        for (const auto& key : getters.getters)
        {
            result.push_back(key.first);
        }
        for (const auto& key : getters.multi_getters)
        {
            result.push_back(key.first);
        }
        return result;
    }

    void Filters2::render()
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
            const auto keys = this->keys(_filter.type_key);
            ImGui::Text("Filters");
            render(_filter, keys, 0, 0, _filter, _filter.type_key);
            ImGui::EndPopup();
        }
        else
        {
            _show_filters = false;
        }
    }

    Filters2::Action Filters2::render(Filter& filter, const std::vector<std::string>& keys, int32_t depth, int32_t index, Filter& parent, const std::string& type_key)
    {
        const std::string current_type_key = filter.type_key != "" ? filter.type_key : type_key;
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
                    if (Action::Remove == render(child, keys, depth + 1, child_index, filter, current_type_key))
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
                        if (has_options(current_type_key, filter.key))
                        {
                            const auto options = options_for_key(current_type_key, filter.key);
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

            auto available_compare_ops = ops_for_key(current_type_key, filter.key);
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

            if (has_options(current_type_key, filter.key) && filter.compare != CompareOp::StartsWith && filter.compare != CompareOp::EndsWith)
            {
                auto available_options = options_for_key(current_type_key, filter.key);
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

    void Filters2::render_settings()
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
                const auto& getters = find_getter(_filter.type_key);
                for (const auto& getter : getters.getters)
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

    void Filters2::scroll_to_item()
    {
        _scroll_to_item = true;
    }

    void Filters2::set_columns(const std::vector<std::string>& columns)
    {
        _columns = columns;
    }

    void Filters2::set_filters(const std::vector<Filter> filters)
    {
        _filter.children = filters;
    }

    void Filters2::set_type_key(const std::string& type_key)
    {
        _filter.type_key = type_key;
    }

    bool Filters2::test_and_reset_changed()
    {
        bool current_value = _changed;
        _changed = false;
        return current_value;
    }

    void Filters2::toggle_visible()
    {
        if (!_show_filters)
        {
            ImGui::OpenPopup(Names::Popup.c_str());
        }
        _show_filters = !_show_filters;
    }
}
