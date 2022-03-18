#pragma once

#include <external/imgui/imgui.h>
#include <external/imgui/misc/cpp/imgui_stdlib.h>
#include <charconv>

namespace trview
{
    template <typename T>
    int Filters<T>::Filter::value_count() const
    {
        switch (compare)
        {
        case CompareOp::Between:
        case CompareOp::BetweenInclusive:
            return 2;
        case CompareOp::Exists:
            return 0;
        }
        return 1;
    }

    template <typename T>
    template <typename value_type>
    void Filters<T>::add_getter(const std::string& key, const std::function<value_type(const T&)>& getter)
    {
        _getters[key] =
        {
            compare_ops<value_type>(),
            [=](const auto& value) { return getter(value); }
        };
    }

    template <typename T>
    template <typename value_type>
    void Filters<T>::add_multi_getter(const std::string& key, const std::function<std::vector<value_type>(const T&)>& getter)
    {
        _multi_getters[key] =
        {
            compare_ops<value_type>(),
            [=](const auto& value)
            {
                const auto results = getter(value);
                std::vector<Value> values;
                std::transform(results.begin(), results.end(), std::back_inserter(values), [](const auto& v) { return v; });
                return values;
            }
        };
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
        return _filters.empty() || std::all_of(_filters.begin(), _filters.end(), [](auto&& f) { return f.key == "" && f.value == ""; });
    }

    template <typename T>
    bool Filters<T>::is_match(const Value& value, const Filter& filter) const
    {
        const std::string* value_string = std::get_if<std::string>(&value);
        if (value_string)
        {
            return is_match(*value_string, filter);
        }
        else
        {
            const float* value_float = std::get_if<float>(&value);
            if (value_float)
            {
                return is_match(*value_float, filter);
            }
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
            bool filter_result = false;

            const auto& getter = _getters.find(filter.key);
            if (getter != _getters.end())
            {
                const auto getter_value = std::get<1>(getter->second)(value);
                filter_result = is_match(getter_value, filter);
            }
            else
            {
                const auto& multi_getter = _multi_getters.find(filter.key);
                if (multi_getter != _multi_getters.end())
                {
                    const auto getter_values = std::get<1>(multi_getter->second)(value);
                    filter_result = std::find_if(getter_values.begin(), getter_values.end(), [&](const auto& value) { return is_match(value, filter); }) != getter_values.end();
                }
            }

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
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();

                auto available_compare_ops = ops_for_key(filter.key);
                if (ImGui::BeginCombo((Names::FilterCompareOp + std::to_string(i)).c_str(), compare_op_to_string(filter.compare).c_str()))
                {
                    for (const auto& compare_op : available_compare_ops)
                    {
                        if (ImGui::Selectable(compare_op_to_string(compare_op).c_str(), compare_op == filter.compare))
                        {
                            filter.compare = compare_op;
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
                if (filter.value_count() > 0)
                {
                    ImGui::InputText((Names::FilterValue + std::to_string(i)).c_str(), &filter.value);
                    ImGui::SameLine();
                }
                if (filter.value_count() > 1)
                {
                    ImGui::InputText((Names::FilterValue + "2-" + std::to_string(i)).c_str(), &filter.value2);
                    ImGui::SameLine();
                }
                if (ImGui::Button((Names::RemoveFilter + std::to_string(i)).c_str()))
                {
                    remove.push_back(i);
                }

                if (i != _filters.size() - 1)
                {
                    std::vector<Op> ops{ Op::And, Op::Or };
                    if (ImGui::BeginCombo((Names::FilterOp + std::to_string(i)).c_str(), op_to_string(filter.op).c_str()))
                    {
                        for (const auto& op : ops)
                        {
                            if (ImGui::Selectable(op_to_string(op).c_str(), op == filter.op))
                            {
                                filter.op = op;
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
            }
            ImGui::EndPopup();
        }
        else
        {
            _show_filters = false;
        }
    }

    template <typename T>
    std::vector<CompareOp> Filters<T>::ops_for_key(const std::string& key) const
    {
        const auto& getter = _getters.find(key);
        if (getter != _getters.end())
        {
            return std::get<0>(getter->second);
        }
        else
        {
            const auto& multi_getter = _multi_getters.find(key);
            if (multi_getter != _multi_getters.end())
            {
                auto ops = std::get<0>(multi_getter->second);
                ops.push_back(CompareOp::Exists);
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
    void Filters<T>::set_filters(const std::vector<Filter> filters)
    {
        _filters = filters;
    }

    constexpr std::string compare_op_to_string(CompareOp op)
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
        }
        return "?";
    }

    constexpr std::string op_to_string(Op op)
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

    template <>
    constexpr std::vector<CompareOp> compare_ops<float>()
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
    constexpr std::vector<CompareOp> compare_ops<std::string>()
    {
        return
        {
            CompareOp::Equal,
            CompareOp::NotEqual
        };
    }
}
