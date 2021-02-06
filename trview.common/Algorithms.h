#pragma once

namespace trview
{
    template < typename T >
    bool equals_any(T value, T other);

    template < typename T, typename... Args >
    bool equals_any(T value, T other, Args... set);
}

#include "Algorithms.hpp"
