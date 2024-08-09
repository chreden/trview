# Scriptable

The Scriptable library is used to create objects that are integrated into TRView and can be customised by plugins.

# Properties

| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| data | Any | RW | User (script) settable custom data storage |
| notes | string | RW | Notes to appear in the viewer when visible |
| on_click | `function()` | W | Function to call when user clicks on the scriptable in the viewer |
| on_tooltip | `function() -> string` | W | Function to call when user hovers over the scriptable |
| position | [Vector3](vector3.md) | RW | Position in the level |

# Non-Instance Functions

| Name | Returns | Parameters | Description |
| ---- | ------- | ---------- | ----------- |
| new | [Scriptable](scriptable.md) | | Create a new Scriptable |
