#pragma once

#include <SimpleMath.h>

namespace trview
{
    struct PickResult
    {
        enum class Type
        {
            Room,
            Entity,
            Mesh,
            Trigger,
            Waypoint
        };

        bool                         hit{ false };
        float                        distance{ FLT_MAX };
        DirectX::SimpleMath::Vector3 position;
        Type                         type{ Type::Room };
        uint32_t                     index{ 0u };
        bool                         stop{ false };
    };

    /// Convert the pick result to a display string.
    /// @param pick The result to convert.
    std::wstring pick_to_string(const PickResult& pick);
}
