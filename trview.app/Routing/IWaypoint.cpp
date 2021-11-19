#include "IWaypoint.h"

namespace trview
{
    IWaypoint::~IWaypoint()
    {
    }

    IWaypoint::Type waypoint_type_from_string(const std::string& value)
    {
        if (value == "Trigger")
        {
            return IWaypoint::Type::Trigger;
        }

        if (value == "Entity")
        {
            return IWaypoint::Type::Entity;
        }

        return IWaypoint::Type::Position;
    }

    std::wstring waypoint_type_to_string(IWaypoint::Type type)
    {
        switch (type)
        {
            case IWaypoint::Type::Entity:
                return L"Entity";
            case IWaypoint::Type::Position:
                return L"Position";
            case IWaypoint::Type::Trigger:
                return L"Trigger";
        }
        return L"Unknown";
    }
}
