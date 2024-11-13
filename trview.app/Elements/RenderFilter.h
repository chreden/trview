#pragma once

namespace trview
{
    enum class RenderFilter : uint32_t
    {
        None = 0x0,
        Rooms = 0x1,
        Entities = 0x2,
        Triggers = 0x4,
        AllGeometry = 0x8,
        Water = 0x10,
        BoundingBoxes = 0x20,
        Lights = 0x40,
        CameraSinks = 0x80,
        Lighting = 0x100,
        SoundSources = 0x200,
        NgPlus = 0x400,
        All = 0xffffffff,
        Default = Rooms | Entities | Triggers | Water
    };
}

