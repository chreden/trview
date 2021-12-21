#include "ITrigger.h"

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
            { TriggerCommandType::Object, L"Item" },
            { TriggerCommandType::Camera, L"Camera" },
            { TriggerCommandType::UnderwaterCurrent, L"Current" },
            { TriggerCommandType::FlipMap, L"Flip Map" },
            { TriggerCommandType::FlipOn, L"Flip On" },
            { TriggerCommandType::FlipOff, L"Flip Off" },
            { TriggerCommandType::LookAtItem, L"Look at Item" },
            { TriggerCommandType::EndLevel, L"End Level" },
            { TriggerCommandType::PlaySoundtrack, L"Music" },
            { TriggerCommandType::Flipeffect, L"Flipeffect" },
            { TriggerCommandType::SecretFound, L"Secret" },
            { TriggerCommandType::ClearBodies, L"Clear Bodies" },
            { TriggerCommandType::Flyby, L"Flyby" },
            { TriggerCommandType::Cutscene, L"Cutscene" }
        };

        const std::unordered_map<std::wstring, TriggerCommandType> command_type_lookup
        {
            { L"Object", TriggerCommandType::Object },
            { L"Camera", TriggerCommandType::Camera },
            { L"Current", TriggerCommandType::UnderwaterCurrent },
            { L"Flip Map", TriggerCommandType::FlipMap },
            { L"Flip On", TriggerCommandType::FlipOn },
            { L"Flip Off", TriggerCommandType::FlipOff },
            { L"Look at Item", TriggerCommandType::LookAtItem },
            { L"End Level", TriggerCommandType::EndLevel },
            { L"Music", TriggerCommandType::PlaySoundtrack },
            { L"Flipeffect", TriggerCommandType::Flipeffect },
            { L"Secret", TriggerCommandType::SecretFound },
            { L"Clear Bodies", TriggerCommandType::ClearBodies },
            { L"Flyby", TriggerCommandType::Flyby },
            { L"Cutscene", TriggerCommandType::Cutscene }
        };
    }

    ITrigger::~ITrigger()
    {
    }

    bool has_command(const ITrigger& trigger, TriggerCommandType type)
    {
        const auto commands = trigger.commands();
        return std::any_of(commands.begin(), commands.end(), [&](const auto& c) { return c.type() == type; });
    }

    bool has_any_command(const ITrigger& trigger, const std::vector<TriggerCommandType>& types)
    {
        return std::any_of(types.begin(), types.end(), [&](const auto& type) { return has_command(trigger, type); });
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

    TriggerCommandType command_from_name(const std::wstring& name)
    {
        auto type = command_type_lookup.find(name);
        if (type == command_type_lookup.end())
        {
            return TriggerCommandType::Object;
        }
        return type->second;
    }

    bool command_has_index(TriggerCommandType type)
    {
        return !(type == TriggerCommandType::ClearBodies || type == TriggerCommandType::EndLevel);
    }

    bool command_is_item(TriggerCommandType type)
    {
        switch (type)
        {
        case TriggerCommandType::Object:
        case TriggerCommandType::Camera:
        case TriggerCommandType::LookAtItem:
            return true;
        }
        return false;
    }
}
