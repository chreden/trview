module trview.app:PickResult;

import std;

import trview.common;

import :ILevel;
import :IItem;
import :ITrigger;
import :ILight;
import :IRoom;
import :IWaypoint;
import :ICameraSink;
import :ISoundSource;
import :IScriptable;
import :IFlybyNode;

namespace trview
{
    Colour pick_to_colour(const PickResult& pick)
    {
        switch (pick.type)
        {
        case PickResult::Type::Entity:
        case PickResult::Type::Waypoint:
            return Colour(0.0f, 1.0f, 0.0f);
        case PickResult::Type::Trigger:
            return Colour(1.0f, 0.0f, 1.0f);
        }
        return Colour(1.0f, 1.0f, 1.0f);
    }

    PickResult nearest_result(const PickResult& current, const PickResult& next)
    {
        if (next.hit && next.distance < current.distance)
        {
            return next;
        }
        return current;
    }
 }