#include "Trigger.h"
#include <trview/Types.h>

namespace trview
{
    Trigger::Trigger(uint32_t number, uint16_t room, uint16_t x, uint16_t z, const TriggerInfo& trigger_info)
        : _number(number), _room(room), _x(x), _z(z)
    {
        for (auto action : trigger_info.commands)
        {
            if (action.first == TriggerCommandType::Object)
            {
                _objects.push_back(action.second);
            }
        }
    }

    uint32_t Trigger::number() const
    {
        return _number;
    }

    uint16_t Trigger::room() const
    {
        return _room;
    }

    uint16_t Trigger::x() const
    {
        return _x;
    }

    uint16_t Trigger::z() const
    {
        return _z;
    }

    bool Trigger::triggers_item(uint16_t index) const
    {
        return std::find(_objects.begin(), _objects.end(), index) != _objects.end();
    }
}
