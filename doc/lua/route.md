# Route

The Route library lets you create and edit routes. Routes need to be bound to a level for the references in waypoints to be correctly bound, unless they have been set directly. This can be done by setting the level property or making the route the current route in the application.

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| colour | [Colour](colour.md) | RW | The route line colour |
| filename | string | R | The file to read and write from/to |
| level | [Level](level.md) | RW | The level all targets will reference |
| waypoints | [Waypoint](waypoint.md)[] | RW | The route waypoints |
| waypoint_colour | [Colour](colour.md) | RW | The default waypoint colour |
| is_randomizer | boolean | R | Whether this is a randomizer route |

# Non-Instance Functions

| Name | Returns | Parameters | Description |
| ---- | ------- | ---------- | ----------- |
| new | Route | ```[optional] { boolean is_randomizer }``` | Creates a new route. Is `is_randomizer` is true the route will be a randomizer route. |
| open | Route | [optional] { [optional] string filename, [optional] boolean is_randomizer, [Level](level.md) level } | Imports a route. If no paramters are supplied or the `filename` property is empty the user will be prompted to choose a file to import. If `is_randomizer` is true then the import will default to Randomizer json files. The `level` parameter is required as the user may attempt to load a Randomizer json which will use the level to adjust some coordinates. If the filename is provided the user will only be prompted if they wish to allow the import. |

# Functions

| Name | Returns | Parameters | Description |
| ---- | ------- | ---------- | ----------- |
| add | [Waypoint](waypoint.md) | [Waypoint](waypoint.md) | Add the specified waypoint to the end of the route |
| clear | - | - | Removes all waypoints in the route |
| save | - | `[optional] { [optional] string filename }` | Export the route. If no parameters table is specified or the `filename` property is not set on the table the user will be prompted to choose a filename. If route is a randomizer route then it will be saved as such. |
| save_as | - | `[optional] { [optional] string filename }` | Export the route. If no parameters table is specified or the `filename` property is not set on the table the user will be prompted to choose a filename. If route is a randomizer route then it will be saved as such. |
| reload | - | - | Reload the file from the stored filename |
| remove | - | [Waypoint](waypoint.md) | Remove the specified waypoint from the route |
