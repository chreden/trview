# Route

The Route library lets you create and edit routes.

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| colour | [Colour](colour.md) | RW | The route line colour |
| waypoint_colour | [Colour](colour.md) | RW | The default waypoint colour |

# Non-Instance Functions

| Name | Returns | Parameters | Description |
| ---- | ------- | ---------- | ----------- |
| new | Route | - | Creates a new route |

# Functions

| Name | Returns | Parameters | Description |
| ---- | ------- | ---------- | ----------- |
| add | - | `{ Item item, Trigger trigger, Vector3 position }` | Adds either an [Item](item.md), [Trigger](trigger.md) or [Position](vector3.md) to the end of route, in that order of precedence. Only one needs to be provided. |
| clear | - | - | Removes all waypoints in the route |