# Triangle

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| animation_mode | [AnimationMode](#animationmode) | R | |
| collision_mode | [CollisionMode](#collisionmode) | R | |
| colours | [Colour](colour.md)[] | R | |
| current_frame | Number | R | |
| current_time | Number | R | |
| frames | [Frame](frame.md)[] | R | |
| frame_time | Number | R | |
| normal | [Vector3](vector3.md) | R | |
| normals | [Vector3](vector3.md)[] | R | |
| position | [Vector3](vector3.md) | R | |
| side_mode | [SideMode](#sidemode) | R | |
| texture | Number | R | |
| texture_mode | [TextureMode](#texturemode) | R | |
| transform | [Matrix](matrix.md) | R | |
| transparency_mode | [TransparencyMode](#transparencymode) | R | |
| uv | [Vector2](vector2.md)[] | R | |
| vertices | [Vector3](vector3.md)[] | R | |

# Constructor

| Parameters | Description |
|  | |

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