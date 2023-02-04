# Level

The Level library provides information about the currently loaded Level in trview.

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| alternate_mode | boolean | RW | Whether the level flipmap is enabled (TR1-3) |
| cameras_and_sinks | [CameraSink](camera_sink.md)[] | R | All cameras/sinks in the level |
| filename | string | R | The path of the level file |
| floordata | Table | R | All floordata |
| items | [Item](item.md)[] | R | All items |
| lights | [Light](light.md)[] | R | All lights |
| rooms | [Room](room.md)[] | R | All rooms |
| selected_room | [Room](room.md) | RW | Currently selected room |
| triggers | [Trigger](trigger.md)[] | R | All triggers |
| version | number | R | The game number for which this level was made |