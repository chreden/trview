# Triangle

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| animation_mode | [AnimationMode](#animationmode) | R | |
| collision_mode | [CollisionMode](#collisionmode) | R | |
| colours | [Colour](colour.md)[] | R | |
| current_frame | Number | R | |
| current_time | Number | R | |
| frames | [Frame](frame.md)[] | R | Not yet supported |
| frame_time | Number | R | |
| normal | [Vector3](vector3.md) | R | |
| normals | [Vector3](vector3.md)[] | R | |
| position | [Vector3](vector3.md) | R | |
| side_mode | [SideMode](#sidemode) | R | |
| texture | Number | R | |
| texture_mode | [TextureMode](#texturemode) | R | |
| transform | [Matrix](matrix.md) | R | Not yet supported  |
| transparency_mode | [TransparencyMode](#transparencymode) | R | |
| uv | [Vector2](vector2.md)[] | R | Not yet supported |
| vertices | [Vector3](vector3.md)[] | R | |

# Constructor

| Parameters | Description |
| ---------- | ----------- |
| `{ [optional] AnimationMode animation_mode, [optional] CollisionMode collision_mode, [optional] Colour[] colours, [optional] number current_frame, [optional] number current_time, [optional] Vector3[] normals, [optional] SideMode side_mode, [optional] number texture, [optional] TextureMode texture_mode, [optional] TransparencyMode transparency_mode, [optional] Vector3[] vertices }` | |

# Non-Instance Functions

| Name | Returns | Parameters | Description |
| ---- | ------- | ---------- | ----------- |

# Instance Functions

# Enumerations

## AnimationMode

```Sector.AnimationMode```

| Name | Value |
| ---- | ----- |
| None | 0 |
| Swap | 1 |
| UV | 2 |

## CollisionMode

```Sector.CollisionMode```

| Name | Value |
| ---- | ----- |
| Disabled | 0 |
| Enabled | 1 |

## SideMode

```Sector.SideMode```

| Name | Value |
| ---- | ----- |
| Single | 0 |
| Double | 1 |

## TextureMode

```Sector.TextureMode```

| Name | Value |
| ---- | ----- |
| Textured | 0 |
| Untextured | 1 |

## TransparencyMode

```Sector.TransparencyMode```

| Name | Value |
| ---- | ----- |
| None | 0 |
| Normal | 1 |
| Additive | 2 |