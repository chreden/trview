#include "PickResult.h"
#include "Compass.h"
#include <string>
#include <trview.common/Colour.h>

namespace trview
{
    namespace
    {
        std::wstring pick_type_to_string(PickResult::Type type)
        {
            switch (type)
            {
            case PickResult::Type::Entity:
                return L"I";
            case PickResult::Type::Trigger:
                return L"T";
            case PickResult::Type::Room:
                return L"R";
            case PickResult::Type::Waypoint:
                return L"W";
            }
            return L"?";
        }

        std::wstring axis_to_string(Compass::Axis axis)
        {
            switch (axis)
            {
            case Compass::Axis::Pos_X:
                return L"+X";
            case Compass::Axis::Pos_Y:
                return L"+Y";
            case Compass::Axis::Pos_Z:
                return L"+Z";
            case Compass::Axis::Neg_X:
                return L"-X";
            case Compass::Axis::Neg_Y:
                return L"-Y";
            case Compass::Axis::Neg_Z:
                return L"-Z";
            }
            return L"?";
        }
    }

    std::wstring pick_to_string(const PickResult& pick)
    {
        if (!pick.text.empty())
        {
            return pick.text;
        }

        if (pick.type == PickResult::Type::Compass)
        {
            return axis_to_string(static_cast<Compass::Axis>(pick.index));
        }

        return pick_type_to_string(pick.type) + L"-" + std::to_wstring(pick.index);
    }

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