#pragma once

#include <format>

namespace trview
{
    template <typename T>
    constexpr std::string get_string(T&& value)
    {
        if constexpr (std::is_same<T, std::string>::value)
        {
            return value;
        }
        return std::format("{}", value);
    }

    std::string join(std::ranges::input_range auto&& range)
    {
        std::string result;
        auto iter = range.begin();
        while (iter != range.end())
        {
            result += *iter;
            if (++iter != range.end())
            {
                result += ",";
            }
        }
        return result;
    }
}

