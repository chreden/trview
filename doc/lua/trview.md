# trview

The trview library provides information about the program in general.

# Properties

| Name | Type | Mode | Description |
| ---- | ---- | ---- | ----------- |
| camera | [Camera](camera.md) | R | The current camera |
| level | [Level](level.md) | RW | The current level, or `nil` if no level loaded. Can be set to a `Level` instance. Will raise an error if the level could not be loaded or the user cancelled the operation.   |
| recent_files | string[] | R | The table of recently opened filenames |
| local_levels | string[] | R | Filenames of levels that are in the level switcher |
| route | [Route](route.md) | RW | The current route. Setting this property will cause the route to be bound to this level. This will resolve any unresolved references in waypoints. |

# Functions

| Name | Returns | Parameters | Description |
| ---- | ------- | ---------- | ----------- |
| load | [Level](level.md) | `string` filename | Load a level from the file provided. This will not automatically load the level in the viewer. |
