# tr_pc_room_may_1996

Note this is not a fixed length structure.

## Format

| Data Type | Description    |
| ----------| -------------  |
| `int32`   | X position |
| `int32`   | Z position |
| `uint32` | Number of data words for room mesh. See [Room Mesh (May 1996)](tr_pc_room_mesh_may_1996.md). |
| `uint16`[n] | Room mesh data words |
| `uint16` | Number of portals |
| [tr_room_portal](tr_room_portal.md)[n] | Portals |
| `uint16` | Number of Z sectors |
| `uint16` | Number of X sectors |
| [tr_room_sector](tr_room_sector.md)[z * x] | Sectors |
| `int16` | Ambient Intensity |
| `uint16` | Number of lights |
| [tr_room_light](tr_room_light.md)[n] | Lights |
| `uint16` | Number of Static Meshes |
| [tr_room_staticmesh](tr_room_staticmesh.md)[n] | Static meshes |
| `int16` | Flags |