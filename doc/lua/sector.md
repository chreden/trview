# Sector

The Sector library provides information about a sector in a [Level](level.md)/[Room](room.md).

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| above | [Room](room.md) | R | Room directly above this sector or `nil` |
| below | [Room](room.md) | R | Room directly below this sector or `nil` |
| ceiling_corners | number[] | R | Height of ceiling corners in clicks from room ceiling |
| ceiling_triangulation | string | R | Ceiling triangulation direction (`None`/`NWSE`/`NESW`) |
| corners | number[] | R | Height of corners in clicks above room floor |
| flags | [Flags](#flags)| R | Sector flags |
| floordata | Table of number | R | Raw floordata values for the sector |
| number | number | R | Sector number |
| portal | [Room](room.md) | R | Destination room for portal or `nil` |
| room | [Room](room.md) | R | Room that the sector is in |
| tilt_x | Number | R | X tilt value for the sector. Only check if `FloorSlant` flag is true |
| tilt_z | Number | R | Z tilt value for the sector. Only check if `FloorSlant` flag is true |
| triangulation | string | R | Triangulation direction (`None`/`NWSE`/`NESW`) |
| trigger | [Trigger](trigger.md) | R | Trigger on this sector or `nil` |
| x | number | R | Sector X location in room |
| z | number | R | Sector Z location in room |

# Functions

| Name | Returns | Parameters | Description |
| ---- | ------- | ---------- | ----------- |
| has_flag | boolean | [Flags](#flags) flag | Check whether the flag pattern passed in matches the sector flags - uses bitwise and operation |

# Enumerations

## Flags

```Sector.Flags```

| Name | Value |
| ---- | ----- |
| None | 0x0 |
| Portal | 0x1 |
| Wall | 0x2 |
| Trigger | 0x4 |
| Death | 0x8 |
| FloorSlant | 0x10 |
| CeilingSlant | 0x20 |
| ClimbableNorth | 0x40 |
| ClimbableEast | 0x80 |
| ClimbableSouth | 0x100 |
| ClimbableWest | 0x200 |
| Climbable | 0x3C0 |
| MonkeySwing | 0x400 |
| RoomAbove | 0x800 |
| RoomBelow | 0x1000 |
| MinecartLeft |  0x2000 |
| MinecartRight | 0x4000 |
| SpecialWall | 0x8000 |