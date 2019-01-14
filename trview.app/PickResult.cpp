#include "PickResult.h"
#include <string>

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
    }

    std::wstring pick_to_string(const PickResult& pick)
    {
        return pick_type_to_string(pick.type) + L"-" + std::to_wstring(pick.index);
    }
 }