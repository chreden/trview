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

    template <Enum T>
    bool has_flag(T field, T flag)
    {
        return (field & flag) == flag;
    }

    template <Enum T>
    T operator | (T left, T right)
    {
        using under_t = std::underlying_type_t<T>;
        return static_cast<T>(static_cast<under_t>(left) | static_cast<under_t>(right));
    }

    template <Enum T>
    T& operator |= (T& left, T right)
    {
        left = left | right;
        return left;
    }

    template <Enum T>
    T operator & (T left, T right)
    {
        using under_t = std::underlying_type_t<T>;
        return static_cast<T>(static_cast<under_t>(left) & static_cast<under_t>(right));
    }

    template <Enum T>
    T operator ~ (T left)
    {
        using under_t = std::underlying_type_t<T>;
        return static_cast<T>(~static_cast<under_t>(left));
    }


}
