# Level

The Level library provides information about the currently loaded Level in trview.

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| cameras_and_sinks | Table of Camera/Sinks | R | All cameras/sinks in the level |
| filename | string | R | The path of the level file |
| floordata | Table | R | All floordata |
| items | [Item](item.md)[] | R | All items |
| lights | table of Lights | R | All lights |
| rooms | [Room](room.md) | R | All rooms |
| triggers | [Trigger](trigger.md) | R | All triggers |
| version | number | R | The game number for which this level was made |