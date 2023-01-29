# Light

The Light library provides information about a light in a [level](level.md). Depending on the `type` property and level version different property sets are available.

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| number | number | R | Item number |
| room | [Room](room.md) | R | The room that the item is in |
| type | string | R | The light type name (Sun, Point, Spot, Shadow, Fog Bulb) |
| visible | boolean | RW | Whether the item is visible in the viewer |

# Sun Properties

| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| colour | [Colour](colour.md) | R | Light colour (TR3-5 only) |
| direction | [Vector3](vector3.md) | R | Direction vector of the light. Reversed in game. |

# Point Properties

| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| colour | [Colour](colour.md) | R | Light colour (TR3-5 only) |
| fade | number | R | Fade value (TR1-3 only) |
| falloff | number | R | Falloff value (TR4-5 only) |
| hotspot | number | R | Hotspot value (TR4-5) |
| intensity | number | R | Intensity value (TR1-4 only) |
| position | [Vector3](vector3.md) | R | Position of the object in game units

# Spot Properties

| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| colour | [Colour](colour.md) | R | Light colour (TR3-5 only) |
| cutoff | number | R | Cutoff value (TR4 only) |
| direction | [Vector3](vector3.md) | R | Direction vector of the light. Reversed in game. |
| fade | number | R | Fade value (TR1-3 only) |
| falloff_angle | number | R | Falloff angle (TR4-5 only) |
| hotspot | number | R | Hotspot value (TR4-5) |
| intensity | number | R | Intensity value (TR1-4 only) |
| length | number | R | Length value (TR4 only)
| position | [Vector3](vector3.md) | R | Position of the object in game units
| rad_in | number | R | Rad in value (TR5 only) |
| rad_out | number | R | Rad out value (TR5 only) |
| range | number | R | Range value (TR5 only) |

# Shadow Properties

| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| falloff | number | R | Falloff value (TR4-5 only) |
| hotspot | number | R | Hotspot value (TR4-5) |
| intensity | number | R | Intensity value (TR1-4 only) |
| position | [Vector3](vector3.md) | R | Position of the object in game units

# Fog Bulb Properties

| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| colour | [Colour](colour.md) | R | Light colour (TR5 only) |
| density | number | R | Density value |
| intensity | number | R | Intensity value (TR1-4 only) |
| position | [Vector3](vector3.md) | R | Position of the object in game units
| radius | number | R | Radius value |