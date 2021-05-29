#pragma once

namespace trview
{
    template <typename T1, typename T2>
    bool equals_any(T1&& value, T2&& other)
    {
        return value == other;
    }

    template <typename T1, typename T2, typename... Args>
    bool equals_any(T1&& value, T2&& other, Args&&... set)
    {
        return equals_any(value, other) || equals_any(value, set...);
    }
}
