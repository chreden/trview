#pragma once

#include <cstdint>
#include <vector>

namespace trview
{
    struct TriggerInfo;

    enum class TriggerType
    {
        Trigger, Pad, Switch, Key, Pickup, HeavyTrigger, Antipad, Combat, Dummy,
        AntiTrigger, HeavySwitch, HeavyAntiTrigger, Monkey, Skeleton, Tightrope, Crawl, Climb
    };

    class Trigger final 
    {
    public:
        explicit Trigger(uint32_t number, uint16_t room, uint16_t x, uint16_t z, const TriggerInfo& trigger_info);

        uint32_t    number() const;
        uint16_t    room() const;
        uint16_t    x() const;
        uint16_t    z() const;
        bool        triggers_item(uint16_t index) const;
        TriggerType type() const;
        bool        only_once() const;
        uint16_t    flags() const;
        uint8_t     timer() const;
    private:
        std::vector<uint16_t> _objects;
        TriggerType _type;
        uint32_t _number;
        uint16_t _room;
        uint16_t _x;
        uint16_t _z;
        bool _only_once;
        uint16_t _flags;
        uint8_t _timer;
    };

    /// Get the string representation of the trigger type specified.
    /// @param type The type to test.
    /// @returns The string version of the enum.
    std::wstring trigger_type_name(TriggerType type);
}