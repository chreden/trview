#pragma once

namespace trview
{
    /// <summary>
    /// Target filters for picking.
    /// </summary>
    enum class PickFilter : uint32_t
    {
        None = 0x0,
        Geometry = 0x1,
        Entities = 0x2,
        Triggers = 0x4,
        AllGeometry = 0x8,
        StaticMeshes = 0x10,
        Lights = 0x20,
        CameraSinks = 0x40,
        NgPlus = 0x80,
        All = 0xffffffff,
        Default = All & ~(AllGeometry)
    };
}
