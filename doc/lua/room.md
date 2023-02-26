# Room

The Room library provides information about a room in a [level](level.md).

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| alternate_mode | string | R | none/has_alternate/is_alternate |
| alternate_group | number | R | Flipmap number (TR4+) |
| alternate_room | [Room](room.md) | R | Alternate room or nil if not present |
| cameras_and_sinks | [CameraSink](camera_sink.md)[] | R | Camera/Sinks in the room |
| items | [Item](item.md)[] | R | Items in the room |
| level | [Level](level.md) | R | The level that the room is in |
| lights | [Light](light.md)[] | R | Lights in the room |
| number | number | R | Room number |
| sectors | [Sector](sector.md)[] | R | Sectors in the room
| triggers | [Trigger](trigger.md)[] | R | Triggers in the room |
| visible | boolean | RW | Whether the room is visible in the viewer |

# Functions

| Name | Returns | Parameters | Description |
| ---- | ------- | ---------- | ----------- |
| sector | [Sector](sector.md) | number x, number y | Gets the sector at the x and z indices, 1 based |
