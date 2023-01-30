# CameraSink

The CameraSink library provides information about a camera/sink in a [level](level.md).

Cameras and Sinks share a data type, so this type provides access to both sets of properties. To find out which type trview thinks it is, check the `type` property. trview will check for `Current` triggers and `Camera` triggers to determine the type - but if a camera/sink is not referenced by any trigger it may not have the correct type. The `type` property is writable and will change the type in the viewer.

# Camera Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| flag | number | R | Flag value |
| number | number | R | Camera/Sink number |
| persistent | boolean | R | If true, player can't cancel the camera with guns/look |
| position | [Vector3](vector3.md) | R | Position of the camera/sink |
| room | [Room](room.md) | R | Room that contains the Camera |
| type | string | RW | Type: `Camera` or `Sink` |
| triggered_by | [Trigger](trigger.md)[] | R | Triggers that reference the camera | 
| visible | boolean | RW | Whether the room is visible in the viewer |

# Sink Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| box_index | number | R | Box index |
| inferred_rooms | [Room](room.md)[] | R | Rooms that trview thinks this sink is in |
| number | number | R | Camera/Sink number |
| position | [Vector3](vector3.md) | R | Position of the camera/sink |
| strength | number | R | How quickly this moves Lara |
| type | string | RW | Type: `Camera` or `Sink` |
| triggered_by | [Trigger](trigger.md)[] | R | Triggers that reference the sink | 
| visible | boolean | RW | Whether the room is visible in the viewer |
