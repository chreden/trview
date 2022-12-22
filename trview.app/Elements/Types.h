#pragma once

#include <vector>

namespace trview
{
    // Flags stored on sector to determine behaviour 
    enum class SectorFlag
    {
        None = 0x0,
        Portal = 0x1,
        SpecialWall = 0x2,
        Wall = 0x4,
        Trigger = 0x8,
        Death = 0x10,
        FloorSlant = 0x20,
        CeilingSlant = 0x40,
        ClimbableNorth = 0x80, // Top edge is climbable 
        ClimbableEast = 0x100, // Right edge is climbable 
        ClimbableSouth = 0x200, // Bottom edge is climbable 
        ClimbableWest = 0x400, // Left edge is climbable
        MonkeySwing = 0x800,
        RoomAbove = 0x1000, // There is a ceiling portal above
        RoomBelow = 0x2000, // There is a floor portal 
        MinecartLeft = 0x4000, // Minecart turns left, Trigger Triggerer in TR4+
        MinecartRight = 0x8000, // Minecart turns right,
        Climbable = ClimbableNorth | ClimbableEast | ClimbableSouth | ClimbableWest
    };

    enum ClimbableWalls
    {
        Top = 0x1, 
        Right = 0x2, 
        Bottom = 0x4, 
        Left = 0x8
    };

    enum class TriggerType
    {
        Trigger, Pad, Switch, Key, Pickup, HeavyTrigger, Antipad, Combat, Dummy,
        AntiTrigger, HeavySwitch, HeavyAntiTrigger, Monkey, Skeleton, Tightrope, Crawl, Climb
    };

    enum class TriggerCommandType
    {
        Object, Camera, UnderwaterCurrent, FlipMap, FlipOn, FlipOff, LookAtItem,
        EndLevel, PlaySoundtrack, Flipeffect, SecretFound, ClearBodies, Flyby, Cutscene
    };

    struct TriggerInfo
    {
        std::uint8_t timer, oneshot, mask;
        TriggerType type; 
        uint16_t sector_id;
        std::vector<std::pair<TriggerCommandType, std::uint16_t>> commands;
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
    
}