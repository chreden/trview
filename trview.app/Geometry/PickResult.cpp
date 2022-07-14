#include "PickResult.h"
#include <trview.app/Tools/Compass.h>
#include <trview.common/Colour.h>
#include "../Elements/ILevel.h"
#include "../Routing/IRoute.h"

namespace trview
{
    namespace
    {
        std::wstring pick_type_to_string(PickResult::Type type)
        {
            switch (type)
            {
            case PickResult::Type::Entity:
                return L"Item";
            case PickResult::Type::Trigger:
                return L"Trigger";
            case PickResult::Type::Room:
                return L"Room";
            case PickResult::Type::Waypoint:
                return L"Waypoint";
            case PickResult::Type::Light:
                return L"Light";
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

        return pick_type_to_string(pick.type) + L" " + std::to_wstring(pick.index);
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

    std::wstring generate_pick_message(const PickResult& result, const ILevel& level, const IRoute& route)
    {
        std::wstringstream stream;

        switch (result.type)
        {
            case PickResult::Type::Entity:
            {
                const auto item = level.item(result.index);
                if (item.has_value())
                {
                    stream << L"Item " << result.index << L" - " << item.value().type();
                }
                break;
            }
            case PickResult::Type::Trigger:
            {
                if (auto trigger = level.trigger(result.index).lock())
                {
                    stream << trigger_type_name(trigger->type()) << L" " << result.index;
                    for (const auto command : trigger->commands())
                    {
                        stream << L"\n  " << command_type_name(command.type());
                        if (command_has_index(command.type()))
                        {
                            stream << L" " << command.index();
                            if (command_is_item(command.type()))
                            {
                                const auto item = level.item(command.index());
                                stream << L" - " << (item.has_value() ? item.value().type() : L"No Item");
                            }
                        }
                    }
                }
                break;
            }
            case PickResult::Type::Light:
            {
                if (const auto light = level.light(result.index).lock())
                {
                    stream << L"Light " << result.index << L" - " << light_type_name(light->type());
                }
                break;
            }
            case PickResult::Type::Room:
            {
                stream << pick_to_string(result);
                break;
            }
            case PickResult::Type::Waypoint:
            {
                auto& waypoint = route.waypoint(result.index);
                stream << L"Waypoint " << result.index;

                if (waypoint.type() == IWaypoint::Type::Entity)
                {
                    const auto item = level.item(waypoint.index());
                    if (item.has_value())
                    {
                        stream << L" - " << item.value().type();
                    }
                }
                else if (waypoint.type() == IWaypoint::Type::Trigger)
                {
                    if (const auto trigger = level.trigger(waypoint.index()).lock())
                    {
                        stream << L" - " << trigger_type_name(trigger->type()) << L" " << waypoint.index();
                    }
                }

                const auto notes = waypoint.notes();
                if (!notes.empty())
                {
                    stream << L"\n\n" << notes;
                }
                break;
            }
            case PickResult::Type::Compass:
            {
                stream << result.text;
                break;
            }
        }

        return stream.str();
    }
 }