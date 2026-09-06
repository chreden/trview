export module trview.app:ILightFunctions;

import std.compat;
import :ILight;
import :IRoom;

namespace trview
{
    export uint32_t light_room(const ILight& light)
    {
        if (auto room = light.room().lock())
        {
            return room->number();
        }
        return 0u;
    }

    export uint32_t light_room(const std::shared_ptr<ILight>& light)
    {
        if (!light)
        {
            return 0u;
        }
        return light_room(*light);
    }
}