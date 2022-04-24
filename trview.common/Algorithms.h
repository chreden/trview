#pragma once

#include <type_traits>

namespace trview
{
    template <typename T1, typename T2>
    bool equals_any(T1&& value, T2&& other);

    template <typename T1, typename T2, typename... Args>
    bool equals_any(T1&& value, T2&& other, Args&&... set);

    template <typename T>
    concept Enum = !std::is_convertible<T, int>::value && std::integral_constant<bool, std::is_enum<T>::value>::value;

    template <Enum T>
    bool has_flag(T field, T flag);

    template <Enum T>
    T operator | (T left, T right);

    template <Enum T>
    T& operator |= (T& left, T right);

    template <Enum T>
    T operator & (T left, T right);

    template <Enum T>
    T operator ~ (T left);
}

#include "Algorithms.hpp"
