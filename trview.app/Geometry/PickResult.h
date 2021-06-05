#pragma once

#include <SimpleMath.h>

namespace trview
{
    struct Colour;

    struct PickResult
    {
        enum class Type
        {
            Room,
            Entity,
            Mesh,
            Trigger,
            Waypoint,
            Compass,
            StaticMesh
        };

        bool                         hit{ false };
        float                        distance{ FLT_MAX };
        DirectX::SimpleMath::Vector3 position;
        Type                         type{ Type::Room };
        uint32_t                     index{ 0u };
        bool                         stop{ false };
        std::wstring                 text;
        bool                         override_centre{ false };
    };

    /// Convert the pick result to a display string.
    /// @param pick The result to convert.
    std::wstring pick_to_string(const PickResult& pick);

    /// Get the appropriate colour for a pick.
    /// @param pick The result to test.
    Colour pick_to_colour(const PickResult& pick);

    /// If the next pick is nearer than the current and is a hit, choose that one.
    PickResult nearest_result(const PickResult& current, const PickResult& next);
}
