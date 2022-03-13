#pragma once

#include <external/imgui/imgui.h>

namespace trview
{
    template <typename T>
    void Filters<T>::add_getter(const std::string& key, const ValueGetter& getter)
    {
        _getters[key] = getter;
    }

    template <typename T>
    std::vector<std::string> Filters<T>::keys() const
    {
        std::vector<std::string> result;
        for (const auto& key : _getters)
        {
            result.push_back(key.first);
        }
        return result;
    }

    template <typename T>
    bool Filters<T>::empty() const
    {
        return filters.empty() || std::all_of(filters.begin(), filters.end(), [](auto&& f) { return f.key == "" && f.value == ""; });
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
        for (const auto& filter : filters)
        {
            const auto& getter = _getters.find(filter.key);
            if (getter != _getters.end())
            {
                const auto getter_value = getter->second(value);
                const auto filter_result =
                    filter.compare == CompareOp::Equal ? getter_value == filter.value :
                                                         getter_value != filter.value;
                match = op == Op::Or ? match | filter_result : match & filter_result;
            }
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
            ImGui::OpenPopup("Filters");
        }
        _show_filters = !_show_filters;
    }

    template <typename T>
    void Filters<T>::render()
    {
        bool filter_enabled = _enabled;
        if (ImGui::Checkbox("##filter_enabled", &filter_enabled))
        {
            _enabled = filter_enabled;
        }
        ImGui::SameLine();

        if (ImGui::Button("Filters"))
        {
            toggle_visible();
        }

        if (_show_filters && ImGui::BeginPopup("Filters"))
        {
            const auto keys = this->keys();

            ImGui::Text("Filters");

            std::vector<uint32_t> remove;
            for (uint32_t i = 0; i < filters.size(); ++i)
            {
                auto& filter = filters[i];
                if (ImGui::BeginCombo(("##filter-key-" + std::to_string(i)).c_str(), filter.key.c_str()))
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
                std::vector<CompareOp> compare_ops{ CompareOp::Equal, CompareOp::NotEqual };
                if (ImGui::BeginCombo(("##filter-compare-op-" + std::to_string(i)).c_str(), compare_op_to_string(filter.compare).c_str()))
                {
                    for (const auto& compare_op : compare_ops)
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
                ImGui::InputText(("##filter-value-" + std::to_string(i)).c_str(), &filter.value);
                ImGui::SameLine();
                if (ImGui::Button(("X##" + std::to_string(i)).c_str()))
                {
                    remove.push_back(i);
                }

                if (i != filters.size() - 1)
                {
                    std::vector<Op> ops{ Op::And, Op::Or };
                    if (ImGui::BeginCombo(("##filter-op-" + std::to_string(i)).c_str(), op_to_string(filter.op).c_str()))
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
                filters.erase(filters.begin() + *iter);
            }

            if (ImGui::Button("+"))
            {
                filters.push_back({});
            }
            ImGui::EndPopup();
        }
        else
        {
            _show_filters = false;
        }
    }

    constexpr std::string compare_op_to_string(CompareOp op)
    {
        switch (op)
        {
        case CompareOp::Equal:
            return "is";
        case CompareOp::NotEqual:
            return "is not";
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
}
