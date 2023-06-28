# Route

The Route library lets you create and edit routes. Routes need to be bound to a level for the references in waypoints to be correctly bound, unless they have been set directly. This can be done by setting the level property or making the route the current route in the application.

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| colour | [Colour](colour.md) | RW | The route line colour |
| level | [Level](level.md) | RW | The level all targets will reference |
| waypoints | [Waypoint](waypoint.md)[] | RW | The route waypoints |
| waypoint_colour | [Colour](colour.md) | RW | The default waypoint colour |

# Non-Instance Functions

| Name | Returns | Parameters | Description |
| ---- | ------- | ---------- | ----------- |
| new | Route | - | Creates a new route |

# Functions

| Name | Returns | Parameters | Description |
| ---- | ------- | ---------- | ----------- |
| add | [Waypoint](waypoint.md) | [Waypoint](waypoint.md) | Add the specified waypoint to the end of the route |
| clear | - | - | Removes all waypoints in the route |
| export | - | `[optional] { [optional] string filename, [optional] boolean is_randomizer }` | Export the route. If no parameters table is specified or the `filename` property is not set on the table the user will be prompted to choose a filename. If the `is_randomizer` setting is set to true in the parameters table then the file type will default to Randomizer locations. The option to export Randomizer settings will be available to the user if the Randomizer integration is enabled or if the `is_randomizer` parameter is true. |
| remove | - | [Waypoint](waypoint.md) | Remove the specified waypoint from the route |
