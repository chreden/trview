#pragma once

namespace trview
{
    template <typename T1, typename T2>
    bool equals_any(T1&& value, T2&& other);

    template <typename T1, typename T2, typename... Args>
    bool equals_any(T1&& value, T2&& other, Args&&... set);

    template <typename T>
    bool has_flag(T field, T flag);
}

#include "Algorithms.hpp"
