#pragma once

namespace trview
{
    enum class Type
    {
        CameraSink,
        Item,
        Light,
        Room,
        Sector,
        Trigger
    };

    constexpr std::string to_string(Type type) noexcept;
}

#include "Type.inl"
