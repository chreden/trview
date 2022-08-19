#include "ITrigger.h"

namespace trview
{
    namespace
    {
        const std::unordered_map<TriggerType, std::string> trigger_type_names
        {
            { TriggerType::Trigger, "Trigger" },
            { TriggerType::Pad, "Pad" },
            { TriggerType::Switch, "Switch" },
            { TriggerType::Key, "Key" },
            { TriggerType::Pickup, "Pickup" },
            { TriggerType::HeavyTrigger, "Heavy Trigger" },
            { TriggerType::Antipad, "Antipad" },
            { TriggerType::Combat, "Combat" },
            { TriggerType::Dummy, "Dummy" },
            { TriggerType::AntiTrigger, "Antitrigger" },
            { TriggerType::HeavySwitch, "Heavy Switch" },
            { TriggerType::HeavyAntiTrigger, "Heavy Antitrigger" },
            { TriggerType::Monkey, "Monkey" },
            { TriggerType::Skeleton, "Skeleton" },
            { TriggerType::Tightrope, "Tightrope" },
            { TriggerType::Crawl, "Craw" },
            { TriggerType::Climb, "Climb"}
        };

        const std::unordered_map<TriggerCommandType, std::string> command_type_names
        {
            { TriggerCommandType::Object, "Item" },
            { TriggerCommandType::Camera, "Camera" },
            { TriggerCommandType::UnderwaterCurrent, "Current" },
            { TriggerCommandType::FlipMap, "Flip Map" },
            { TriggerCommandType::FlipOn, "Flip On" },
            { TriggerCommandType::FlipOff, "Flip Off" },
            { TriggerCommandType::LookAtItem, "Look at Item" },
            { TriggerCommandType::EndLevel, "End Level" },
            { TriggerCommandType::PlaySoundtrack, "Music" },
            { TriggerCommandType::Flipeffect, "Flipeffect" },
            { TriggerCommandType::SecretFound, "Secret" },
            { TriggerCommandType::ClearBodies, "Clear Bodies" },
            { TriggerCommandType::Flyby, "Flyby" },
            { TriggerCommandType::Cutscene, "Cutscene" }
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

    std::string trigger_type_name(TriggerType type)
    {
        auto name = trigger_type_names.find(type);
        if (name == trigger_type_names.end())
        {
            return "Unknown";
        }
        return name->second;
    }

    std::string command_type_name(TriggerCommandType type)
    {
        auto name = command_type_names.find(type);
        if (name == command_type_names.end())
        {
            return "Unknown";
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

    TriggerCommandType command_from_name(const std::string& name)
    {
        return command_from_name(to_utf16(name));
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
        case TriggerCommandType::LookAtItem:
            return true;
        }
        return false;
    }
}
