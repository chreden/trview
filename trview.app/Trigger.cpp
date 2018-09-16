#include "Trigger.h"
#include <trview/Types.h>

namespace trview
{
    Trigger::Trigger(const TriggerInfo& trigger_info)
    {
        for (auto action : trigger_info.commands)
        {
            if (action.first == TriggerCommandType::Object)
            {
                _objects.push_back(action.second);
            }
        }
    }

    bool Trigger::triggers_item(uint16_t index) const
    {
        return std::find(_objects.begin(), _objects.end(), index) != _objects.end();
    }
}
