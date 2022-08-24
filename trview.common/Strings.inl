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
}

