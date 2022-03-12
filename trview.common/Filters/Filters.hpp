#pragma once

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
    bool Filters<T>::match(const T& value) const
    {
        if (filters.empty())
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
                auto filter_result = getter->second(value) == filter.value;
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
