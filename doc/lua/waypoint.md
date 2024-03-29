# Waypoint

The Waypoint library lets you create and edit route waypoints.

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| colour | [Colour](colour.md) | RW | The route line colour |
| item | [Item](item.md) | RW | The item that is the target of the waypoint |
| normal | [Vector3](vector3.md) | RW | The normal that the waypoint pole is aligned to |
| notes | string | RW | Notes for this waypoint |
| position | [Vector3](vector3.md) | RW | The position that is the target of the waypoint |
| randomizer_settings | Table | RW | The randomizer settings for this waypoint (map of `string` to (`boolean`, `number` or `string`)) |
| room | [Room](room.md) | RW | The room that the waypoint is in |
| room_number | number | RW | The room number that the waypoint is in |
| trigger | [Trigger](trigger.md) | RW | The trigger that is the target of the waypoint |
| type | string | R | One of `Item`, `Trigger`, `Position`. Can be changed by setting `item`, `trigger` or `position`.  |
| waypoint_colour | [Colour](colour.md) | RW | The waypoint pole colour |

# Non-Instance Functions

| Name | Returns | Parameters | Description |
| ---- | ------- | ---------- | ----------- |
| new | Waypoint | `{ Item item, Trigger trigger, Vector3 position, Vector3 normal, Room room, Sector sector }` | Creates an [Item](item.md), [Trigger](trigger.md) or [Position](vector3.md) waypoint, in that order of precedence. Only one needs to be provided. Normal is optional - if not provided the waypoint will be vertical. Room is required only if using Position. Sector can be used in place of Position to create a mid waypoint. |
