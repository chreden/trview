# Trigger

The Trigger library provides information about a trigger in a [level](level.md).

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| colour | [Colour](colour.md) | RW | The colour for the trigger in the viewer |
| commands | [Command](command.md)[] | R | Commands for the trigger |
| flags | number | R | |
| number | number | R | Trigger number |
| only_once | boolean | R | Whether the trigger only fires once |
| position | [Vector3](vector3.md) | R | Position in the world |
| room | [Room](room.md) | R | Room in which the trigger is contained |
| sector | [Sector](sector.md) | R | Sector containing the trigger |
| timer | number | R | |
| type | string | R | Trigger type name | 
| visible | boolean | RW | Whether the trigger is visible in the viewer |