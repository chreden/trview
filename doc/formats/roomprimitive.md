# RoomPrimitive

Enumeration used to identify upcoming primitive type in the combined primitives room mesh format used up to version 26. See [Room Mesh (May 1996)](tr_pc_room_mesh_may_1996.md).

Depending on the value the following primitive will be a [tr_face3](tr_face3.md), [tr_face4](tr_face4.md) or [tr_room_sprite](tr_room_sprite.md).

## Size

2 bytes (`uint16`).

## Values

| Value | Meaning    |
| ----------| -------------  |
| 32 | Transparent Triangle |
| 33 | Transparent Rectangle |
| 34 | Invisible Triangle |
| 35 | Invisible Rectangle |
| 36 | Textured Triangle |
| 37 | Textured Rectangle |
| 39 | Sprite |