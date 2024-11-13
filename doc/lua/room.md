# Room

The Room library provides information about a room in a [level](level.md).

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| alternate_mode | string | R | none/has_alternate/is_alternate |
| alternate_group | number | R | Flipmap number (TR4+) |
| alternate_room | [Room](room.md) | R | Alternate room or nil if not present |
| cameras_and_sinks | [CameraSink](camera_sink.md)[] | R | Camera/Sinks in the room |
| items | [Item](item.md)[] | R | Items in the room, no NG+ swap |
| items_ng | [Item](item.md)[] | R | Items in the room, NG+ swap |
| flags | number | R | Room flags |
| level | [Level](level.md) | R | The level that the room is in |
| lights | [Light](light.md)[] | R | Lights in the room |
| number | number | R | Room number |
| num_x_sectors | number | R | Number of sectors in the x dimension |
| num_z_sectors | number | R | Number of sectors in the z dimension |
| position | [Vector3](vector3.md) | R | World position of the room |
| sectors | [Sector](sector.md)[] | R | Sectors in the room
| static_meshes | [StaticMesh](staticmesh.md)[] | R | Static meshes in the room |
| triggers | [Trigger](trigger.md)[] | R | Triggers in the room |
| visible | boolean | RW | Whether the room is visible in the viewer |

# Functions

| Name | Returns | Parameters | Description |
| ---- | ------- | ---------- | ----------- |
| sector | [Sector](sector.md) | number x, number y | Gets the sector at the x and z indices, 1 based |
| has_flag | boolean | [Flags](#flags) flag | Check whether the flag pattern passed in matches the room flags - uses bitwise and operation |

# Flags

```Room.Flags```

| Name | Value |
| ---- | ----- |
| Water | 0x1 |
| Bit1 | 0x2 |
| Bit2 | 0x4 |
| Outside | 0x8 |
| Bit4 | 0x10 |
| Wind | 0x20 |
| Bit6 | 0x40 |
| Bit7 | 0x80 |
| Quicksand | 0x80 |
| NoLensFlare | 0x80 |
| Caustics | 0x100 |
| WaterReflectivity | 0x200 |
| Bit10 | 0x400 |
| Bit11 | 0x800 |
| Bit12 | 0x1000 |
| Bit13 | 0x2000 |
| Bit14 | 0x4000 |
| Bit15 | 0x8000 |
