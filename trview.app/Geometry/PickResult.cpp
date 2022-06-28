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
                const auto item = level.items()[result.index];
                stream << L"Item " << result.index << L" - " << item.type();
                break;
            }
            case PickResult::Type::Trigger:
            {
                const auto trigger = level.triggers()[result.index];
                if (auto trigger_ptr = trigger.lock())
                {
                    stream << trigger_type_name(trigger_ptr->type()) << L" " << result.index;
                    for (const auto command : trigger_ptr->commands())
                    {
                        stream << L"\n  " << command_type_name(command.type());
                        if (command_has_index(command.type()))
                        {
                            stream << L" " << command.index();
                            if (command_is_item(command.type()))
                            {
                                const auto items = level.items();
                                stream << L" - " << (command.index() < items.size() ? items[command.index()].type() : L"No Item");
                            }
                        }
                    }
                }
                break;
            }
            case PickResult::Type::Light:
            {
                const auto light = level.lights()[result.index];
                if (auto light_ptr = light.lock())
                {
                    stream << L"Light " << result.index << L" - " << light_type_name(light_ptr->type());
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

                const auto level_items = level.items();
                const auto level_triggers = level.triggers();
                if (waypoint.type() == IWaypoint::Type::Entity && waypoint.index() < level_items.size())
                {
                    stream << L" - " << level_items[waypoint.index()].type();
                }
                else if (waypoint.type() == IWaypoint::Type::Trigger && waypoint.index() < level_triggers.size())
                {
                    const auto trigger_ptr = level_triggers[waypoint.index()].lock();
                    stream << L" - " << trigger_type_name(trigger_ptr->type()) << L" " << waypoint.index();
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