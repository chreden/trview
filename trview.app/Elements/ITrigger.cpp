#include "ITrigger.h"
#include "IRoom.h"

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
            { TriggerType::Crawl, "Crawl" },
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
            return std::format("Unknown ({})", static_cast<int>(type));
        }
        return name->second;
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

    uint32_t trigger_room(const std::shared_ptr<ITrigger>& trigger)
    {
        if (!trigger)
        {
            return 0u;
        }
        return trigger_room(*trigger);
    }

    uint32_t trigger_room(const ITrigger& trigger)
    {
        if (auto room = trigger.room().lock())
        {
            return room->number();
        }
        return 0u;
    }
}
