#pragma once

namespace trview
{
    template <typename T1, typename T2>
    constexpr bool equals_any(T1&& value, T2&& other)
    {
        return value == other;
    }

    template <typename T1, typename T2, typename... Args>
    constexpr bool equals_any(T1&& value, T2&& other, Args&&... set)
    {
        return equals_any(value, other) || equals_any(value, set...);
    }

    template <typename T1, typename T2>
    constexpr bool all_equal_to(T1&& value, T2&& other)
    {
        return value == other;
    }

    template <typename T1, typename T2, typename... Args>
    constexpr bool all_equal_to(T1&& value, T2&& other, Args&&... set)
    {
        return all_equal_to(value, other) && all_equal_to(value, set...);
    }

    template <Enum T>
    constexpr bool has_flag(T field, T flag) noexcept
    {
        return (field & flag) == flag;
    }

    template <Enum T>
    constexpr bool has_any_flag(T field, T flag) noexcept
    {
        return has_flag(field, flag);
    }

    template <Enum T, Enum... Args>
    constexpr bool has_any_flag(T field, T flag, Args... flags) noexcept
    {
        return has_any_flag(field, flag) || has_any_flag(field, flags...);
    }

    template <Enum T>
    constexpr T operator | (T left, T right) noexcept
    {
        using under_t = std::underlying_type_t<T>;
        return static_cast<T>(static_cast<under_t>(left) | static_cast<under_t>(right));
    }

    template <Enum T>
    constexpr T& operator |= (T& left, T right) noexcept
    {
        left = left | right;
        return left;
    }

    template <Enum T>
    constexpr T operator & (T left, T right) noexcept
    {
        using under_t = std::underlying_type_t<T>;
        return static_cast<T>(static_cast<under_t>(left) & static_cast<under_t>(right));
    }

    template <Enum T>
    constexpr T& operator &= (T& left, T right) noexcept
    {
        left = left & right;
        return left;
    }

    template <Enum T>
    constexpr T operator ~ (T left) noexcept
    {
        using under_t = std::underlying_type_t<T>;
        return static_cast<T>(~static_cast<under_t>(left));
    }

    template <Enum T>
    constexpr T filter_flag(T filter, bool condition) noexcept
    {
        return condition ? filter : T::None;
    }

    template <Enum T>
    constexpr T set_flag(T value, T flag, bool condition) noexcept
    {
        if (condition)
        {
            return value | flag;
        }
        return value & ~flag;
    }
}
