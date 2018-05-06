#pragma once

#include <list>

namespace trview
{
    // Flags stored on sector to determine behaviour 
    enum SectorFlag
    {
        Portal = 0x1,
        Wall = 0x2, 
        Trigger = 0x4, 
        Death = 0x8,
        FloorSlant = 0x10,
        CeilingSlant = 0x20, 
        ClimbableTop = 0x40, // Top edge is climbable 
        ClimbableRight = 0x80, // Right edge is climbable 
        ClimbableDown = 0x100, // Bottom edge is climbable 
        ClimbableLeft = 0x200, // Left edge is climbable
        MonkeySwing = 0x400, 
        RoomAbove = 0x800, // There is a ceiling portal above
        RoomBelow = 0x1000, // There is a floor portal 
        MinecartLeft = 0x2000, // Minecart turns left, Trigger Triggerer in TR4+
        MinecartRight = 0x4000, // Minecart turns right
    };

    enum ClimbableWalls
    {
        Top = 0x1, 
        Right = 0x2, 
        Bottom = 0x4, 
        Left = 0x8
    };

    enum class TriggerCommandType
    {
        Object, Camera, UnderwaterCurrent, FlipMap, FlipOn, FlipOff, LookAtItem,
        EndLevel, PlaySoundtrack, Flipeffect, SecretFound, ClearBodies
    };

    enum class TriggerType
    {
        Trigger, Pad, Switch, Key, Pickup, HeavyTrigger, Antipad, Combat, Dummy, 
        AntiTrigger, HeavySwitch, HeavyAntiTrigger, Monkey, Skeleton, Tightrope, Crawl, Climb
    };

    struct Trigger
    {
        std::uint8_t timer, oneshot, mask;
        TriggerType type; 
        std::vector<std::pair<TriggerCommandType, std::uint16_t>> commands;
    };
    
}