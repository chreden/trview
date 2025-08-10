#pragma once

#include <SimpleMath.h>
#include "Triangle.h"

namespace trview
{
    struct Colour;
    struct ICameraSink;
    struct IFlybyNode;
    struct IItem;
    struct ILevel;
    struct ILight;
    struct IRoom;
    struct IRoute;
    struct IScriptable;
    struct ISoundSource;
    struct IStaticMesh;
    struct ITrigger;
    struct IWaypoint;

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
        UniTriangle                  triangle;
        std::weak_ptr<IScriptable>   scriptable;
        std::weak_ptr<ISoundSource>  sound_source;
        std::weak_ptr<IItem>         item;
        std::weak_ptr<ICameraSink>   camera_sink;
        std::weak_ptr<ILight>        light;
        std::weak_ptr<IStaticMesh>   static_mesh;
        std::weak_ptr<IRoom>         room;
        std::weak_ptr<ITrigger>      trigger;
        std::weak_ptr<IWaypoint>     waypoint;
        std::weak_ptr<IFlybyNode>    flyby_node;
    };

    /// Get the appropriate colour for a pick.
    /// @param pick The result to test.
    Colour pick_to_colour(const PickResult& pick);

    /// If the next pick is nearer than the current and is a hit, choose that one.
    PickResult nearest_result(const PickResult& current, const PickResult& next);

    std::string generate_pick_message(const PickResult& result, const ILevel& level);
}
