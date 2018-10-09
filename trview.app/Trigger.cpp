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

        const std::unordered_map<TriggerCommandType, std::wstring> command_type_names
        {
            { TriggerCommandType::Object, L"Object" },
            { TriggerCommandType::Camera, L"Camera" },
            { TriggerCommandType::UnderwaterCurrent, L"Underwater Current" },
            { TriggerCommandType::FlipMap, L"Flip Map" },
            { TriggerCommandType::FlipOn, L"Flip On" },
            { TriggerCommandType::FlipOff, L"Flip Off" },
            { TriggerCommandType::LookAtItem, L"Look at Item" },
            { TriggerCommandType::EndLevel, L"End Level" },
            { TriggerCommandType::PlaySoundtrack, L"Play Soundtrack" },
            { TriggerCommandType::Flipeffect, L"Flipeffect" },
            { TriggerCommandType::SecretFound, L"Secret Found" },
            { TriggerCommandType::ClearBodies, L"Clear Bodies" }
        };
    }

    Command::Command(uint32_t number, TriggerCommandType type, uint16_t index)
        : _number(number), _type(type), _index(index)
    {
    }

    uint32_t Command::number() const
    {
        return _number;
    }

    TriggerCommandType Command::type() const
    {
        return _type;
    }

    uint16_t Command::index() const
    {
        return _index;
    }

    Trigger::Trigger(uint32_t number, uint16_t room, uint16_t x, uint16_t z, const TriggerInfo& trigger_info)
        : _number(number), _room(room), _x(x), _z(z), _type(trigger_info.type), _only_once(trigger_info.oneshot), _flags(trigger_info.mask), _timer(trigger_info.timer)
    {
        uint32_t command_index = 0;
        for (auto action : trigger_info.commands)
        {
            _commands.push_back({ command_index++, action.first, action.second });
            // Special case for object still.
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

    const std::vector<Command>& Trigger::commands() const
    {
        return _commands;
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

    std::wstring command_type_name(TriggerCommandType type)
    {
        auto name = command_type_names.find(type);
        if (name == command_type_names.end())
        {
            return L"Unknown";
        }
        return name->second;
    }
}
