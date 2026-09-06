module;

#include <SimpleMath.h>

export module trview.app:PickResult;

import std;
import trview.common;

import :Triangle;
import :IPickable;

namespace trview
{
    export struct PickResult
    {
        enum class Type
        {
            Room,
            Entity,
            Mesh,
            Trigger,
            Waypoint,
            Compass,
            StaticMesh,
            Light,
            CameraSink,
            Scriptable,
            SoundSource,
            FlybyNode
        };

        bool                         hit{ false };
        float                        distance{ FLT_MAX };
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector3 centroid;
        Type                         type{ Type::Room };
        // TODO: Bring back for compass and waypoint index maybe.
        uint32_t                     waypoint_index{ 0u };
        bool                         stop{ false };
        std::string                  text;
        bool                         override_centre{ false };
        Triangle                     triangle;
        std::weak_ptr<IPickable>     element;
    };

    /// Get the appropriate colour for a pick.
    /// @param pick The result to test.
    export Colour pick_to_colour(const PickResult& pick);

    /// If the next pick is nearer than the current and is a hit, choose that one.
    export PickResult nearest_result(const PickResult& current, const PickResult& next);
}
