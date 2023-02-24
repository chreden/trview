# Sector

The Sector library provides information about a sector in a [Level](level.md)/[Room](room.md).

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| number | number | R | Sector number |
| portal | [Room](room.md) | R | Destination room for portal, or `nil` |
| room | [Room](room.md) | R | Room that the sector is in |
| room_above | [Room](room.md) | R | Room directly above this sector or `nil` |
| room_below | [Room](room.md) | R | Room directly below this sector or `nil` |
| trigger | [Trigger](trigger.md) | R | Trigger on this sector or `nil` |
| x | number | R | Sector X location in room |
| z | number | R | Sector Z location in room |