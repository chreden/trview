#include "Type.h"

namespace trview
{
    constexpr std::string to_string(Type type) noexcept
    {
        switch (type)
        {
        case Type::CameraSink:
            return "Camera/Sink";
        case Type::Item:
            return "Item";
        case Type::Light:
            return "Light";
        case Type::Room:
            return "Room";
        case Type::Trigger:
            return "Trigger";
        }
        return "Unknown";
    }
}
