# Room Mesh (May 1996)

This room mesh format is up to and including version 21. The mesh format is known to be different in version 26 but may have changed before that point.

## Relevant Versions

- [Version 9](version9.md)
- [Version 11](version11.md)
- [Version 21](version21.md)

## Format

| Data Type | Description    |
| ----------| -------------  |
| `uint16`  | Number of room vertices |
| [tr_room_vertex](tr_room_vertex.md)[n] | Room vertices |
| `uint16` | Number of primitives |
| --- Repeat n times --- | --- |
| [RoomPrimitive](roomprimitive.md) (`uint16`) | Primitive type |
| [tr_face3](tr_face3.md), [tr_face4](tr_face4.md) or [tr_room_sprite](tr_room_sprite.md) | Triangle, rectangle or room sprite based on primitive type |
| --- End repeat --- | --- |