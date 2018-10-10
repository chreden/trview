#pragma once

#include <cstdint>
#include <vector>
#include "TransparentTriangle.h"

namespace trview
{
    struct TriggerInfo;

    enum class TriggerType
    {
        Trigger, Pad, Switch, Key, Pickup, HeavyTrigger, Antipad, Combat, Dummy,
        AntiTrigger, HeavySwitch, HeavyAntiTrigger, Monkey, Skeleton, Tightrope, Crawl, Climb
    };

    enum class TriggerCommandType
    {
        Object, Camera, UnderwaterCurrent, FlipMap, FlipOn, FlipOff, LookAtItem,
        EndLevel, PlaySoundtrack, Flipeffect, SecretFound, ClearBodies
    };

    class Command final
    {
    public:
        Command(uint32_t number, TriggerCommandType type, uint16_t index);
        uint32_t number() const;
        TriggerCommandType type() const;
        uint16_t index() const;
    private:
        uint32_t _number;
        TriggerCommandType _type;
        uint16_t _index;
    };

    class TransparencyBuffer;
    struct ICamera;

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
        uint16_t    sector_id() const;
        const std::vector<Command>& commands() const;
        const std::vector<TransparentTriangle>& triangles() const;
        void set_triangles(const std::vector<TransparentTriangle>& transparent_triangles);
    private:
        std::vector<uint16_t> _objects;
        std::vector<Command> _commands;
        std::vector<TransparentTriangle> _transparent_triangles;
        TriggerType _type;
        uint32_t _number;
        uint16_t _room;
        uint16_t _x;
        uint16_t _z;
        bool _only_once;
        uint16_t _flags;
        uint8_t _timer;
        uint16_t _sector_id;
    };

    /// Get the string representation of the trigger type specified.
    /// @param type The type to test.
    /// @returns The string version of the enum.
    std::wstring trigger_type_name(TriggerType type);

    /// Get the string representation of the command type specified.
    /// @param type The type to test.
    /// @returns The string version of the enum.
    std::wstring command_type_name(TriggerCommandType type);
}