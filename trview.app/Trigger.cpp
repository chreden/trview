#include "Trigger.h"
#include <trview/Types.h>
#include <unordered_map>

namespace trview
{
    namespace
    {
        const std::unordered_map<TriggerType, std::wstring> trigger_type_names
        {
            { TriggerType::Trigger, L"Trigger" },
            { TriggerType::Pad, L"Pad" },
            { TriggerType::Switch, L"Switch" },
            { TriggerType::Key, L"Key" },
            { TriggerType::Pickup, L"Pickup" },
            { TriggerType::HeavyTrigger, L"Heavy Trigger" },
            { TriggerType::Antipad, L"Antipad" },
            { TriggerType::Combat, L"Combat" },
            { TriggerType::Dummy, L"Dummy" },
            { TriggerType::AntiTrigger, L"Antitrigger" },
            { TriggerType::HeavySwitch, L"Heavy Switch" },
            { TriggerType::HeavyAntiTrigger, L"Heavy Antitrigger" },
            { TriggerType::Monkey, L"Monkey" },
            { TriggerType::Skeleton, L"Skeleton" },
            { TriggerType::Tightrope, L"Tightrope" },
            { TriggerType::Crawl, L"Crawl" },
            { TriggerType::Climb, L"Climb"}
        };
    }


    Trigger::Trigger(uint32_t number, uint16_t room, uint16_t x, uint16_t z, const TriggerInfo& trigger_info)
        : _number(number), _room(room), _x(x), _z(z), _type(trigger_info.type), _only_once(trigger_info.oneshot), _flags(trigger_info.mask), _timer(trigger_info.timer)
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

    TriggerType Trigger::type() const
    {
        return _type;
    }

    bool Trigger::only_once() const
    {
        return _only_once;
    }

    uint16_t Trigger::flags() const
    {
        return _flags;
    }

    uint8_t Trigger::timer() const
    {
        return _timer;
    }

    std::wstring trigger_type_name(TriggerType type)
    {
        auto name = trigger_type_names.find(type);
        if (name == trigger_type_names.end())
        {
            return L"Unknown";
        } 
        return name->second;
    }
}
