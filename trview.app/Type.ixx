export module trview.app:Type;

import std;

namespace trview
{
    export enum class Type
    {
        CameraSink,
        Item,
        Light,
        Room,
        Sector,
        Trigger
    };

    export constexpr std::string to_string(Type type) noexcept;
}

#include "Type.inl"
