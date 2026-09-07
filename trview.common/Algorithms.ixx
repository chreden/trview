export module trview.common:Algorithms;

import std;

namespace trview
{
    export template <typename T1, typename T2>
    constexpr bool equals_any(T1&& value, T2&& other);

    export template <typename T1, typename T2, typename... Args>
    constexpr bool equals_any(T1&& value, T2&& other, Args&&... set);

    export template <typename T1, typename T2>
    constexpr bool all_equal_to(T1&& value, T2&& other);

    export template <typename T1, typename T2, typename... Args>
    constexpr bool all_equal_to(T1&& value, T2&& other, Args&&... set);

    export template <typename T>
    concept Enum = !std::is_convertible<T, int>::value && std::integral_constant<bool, std::is_enum<T>::value>::value;

    export template <Enum T>
    constexpr bool has_flag(T field, T flag) noexcept;

    export template <Enum T>
    constexpr bool has_any_flag(T field, T flag) noexcept;

    export template <Enum T, Enum... Args>
    constexpr bool has_any_flag(T field, T flag, Args... flags) noexcept;

    export template <Enum T>
    constexpr T operator | (T left, T right) noexcept;

    export template <Enum T>
    constexpr T& operator |= (T& left, T right) noexcept;

    export template <Enum T>
    constexpr T operator & (T left, T right) noexcept;

    export template <Enum T>
    constexpr T& operator &= (T& left, T right) noexcept;

    export template <Enum T>
    constexpr T operator ~ (T left) noexcept;

    export template <Enum T>
    constexpr T filter_flag(T filter, bool condition) noexcept;

    export template <Enum T>
    constexpr T set_flag(T value, T flag, bool condition) noexcept;
}

#include "Algorithms.hpp"
