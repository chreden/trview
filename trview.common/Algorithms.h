#pragma once

#include <type_traits>

namespace trview
{
    template <typename T1, typename T2>
    constexpr bool equals_any(T1&& value, T2&& other);

    template <typename T1, typename T2, typename... Args>
    constexpr bool equals_any(T1&& value, T2&& other, Args&&... set);

    template <typename T>
    concept Enum = !std::is_convertible<T, int>::value && std::integral_constant<bool, std::is_enum<T>::value>::value;

    template <Enum T>
    constexpr bool has_flag(T field, T flag) noexcept;

    template <Enum T>
    constexpr bool has_any_flag(T field, T flag) noexcept;

    template <Enum T, Enum... Args>
    constexpr bool has_any_flag(T field, T flag, Args... flags) noexcept;

    template <Enum T>
    constexpr T operator | (T left, T right) noexcept;

    template <Enum T>
    constexpr T& operator |= (T& left, T right) noexcept;

    template <Enum T>
    constexpr T operator & (T left, T right) noexcept;

    template <Enum T>
    constexpr T& operator &= (T& left, T right) noexcept;

    template <Enum T>
    constexpr T operator ~ (T left) noexcept;

    template <Enum T>
    constexpr T filter_flag(T filter, bool condition) noexcept;

    template <Enum T>
    constexpr T set_flag(T value, T flag, bool condition) noexcept;
}

#include "Algorithms.hpp"
