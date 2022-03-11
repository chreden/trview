#pragma once

namespace trview
{
    template <typename T>
    void Filters<T>::add_getter(const std::string& key, const ValueGetter& getter)
    {
        _getters[key] = getter;
    }

    template <typename T>
    bool Filters<T>::match(const T& value) const
    {
        return filters.empty() || std::any_of(filters.begin(), filters.end(),
            [&](auto&& filter)
            {
                // TODO - Decide on empty filter behaviour. Should it be true, false or
                // have a special case for when there are only empty filters in the list?
                if (filter.key == "" || filter.value == "")
                {
                    return false;
                }
                const auto& getter = _getters.find(filter.key);
                if (getter != _getters.end())
                {
                    return getter->second(value) == filter.value;
                }
                return false;
            });
    }
}
