#pragma once

#include <SimpleMath.h>
#include <trview.app/Geometry/Triangle.h>

namespace trview
{
    struct Colour;
    struct IItem;
    struct ILevel;
    struct IRoute;
    struct IScriptable;
    struct ISoundSource;

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
            SoundSource
        };

        bool                         hit{ false };
        float                        distance{ FLT_MAX };
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector3 centroid;
        Type                         type{ Type::Room };
        uint32_t                     index{ 0u };
        bool                         stop{ false };
        std::string                  text;
        bool                         override_centre{ false };
        Triangle                     triangle;
        std::weak_ptr<IScriptable>   scriptable;
        std::weak_ptr<ISoundSource>  sound_source;
        std::weak_ptr<IItem>         item;
    };

    /// Convert the pick result to a display string.
    /// @param pick The result to convert.
    std::string pick_to_string(const PickResult& pick);

    /// Get the appropriate colour for a pick.
    /// @param pick The result to test.
    Colour pick_to_colour(const PickResult& pick);

    /// If the next pick is nearer than the current and is a hit, choose that one.
    PickResult nearest_result(const PickResult& current, const PickResult& next);

    std::string generate_pick_message(const PickResult& result, const ILevel& level, const IRoute& route);
}
