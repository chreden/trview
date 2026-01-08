# Version 11

July 1996 version.

## Format

This format is split into three files: `.SWD`, `.TOM` and `.WAD`.

The WAD and SWD files will be merged into the main file in later versions.

## Changes from Version 11

- Room format changed from [tr_pc_room_may_1996](tr_pc_room_may_1996.md) to [tr_pc_room_21](tr_pc_room_21.md).
  - Full room info added
  - Alternate rooms
- Previous unknown `uint32` confirmed to be cameras.
- Zones added.
- Entity format now the same as retail TR1
  - Intensity1 added
- Floordata functions now the same order as retail TR1

## .TOM Format

| Data Type | Description    | Note |
| ----------| -------------  | ---- |
| `uint32`  | Version number ||
| `uint16`  | Room texture count ||
| [tr_object_texture_may_1996](tr_object_texture_may_1996.md)[n] | Room object textures ||
| `uint32` | Textile size in bytes ||
| `uint8`[n] | Textile bytes ||
| [tr_colour](tr_colour.md)[256] | 8-bit palette ||
| `uint16` | Room count ||
| [tr_pc_room_21](tr_pc_room_21.md)[n] | Rooms | :pencil2: |
| `uint32` | Floordata length ||
| `uint16` | Floordata ||
| `uint32` | Number of cameras. ||
| [tr_camera](tr_camera.md)[n] | Cameras | :sparkles: |
| `uint32` | Number of boxes ||
| [tr_box](tr_box.md)[n] | Boxes ||
| `uint32` | Number of overlaps ||
| `uint16`[n] | Overlaps ||
| `uint32`[boxes * 4] | Zones* | :sparkles: |
| `uint32` | Number of animated texture ranges ||
| [tr_animated_texture_range](tr_animated_texture_range.md) | Animated texture ranges ||
| `uint32` | Number of entities ||
| [tr_entity](tr_entity.md)[n] | Entities | :pencil2: |
| `uint8`[8192] | Light map ||

## .WAD Format

WAD format is shared with several later versions. See [WAD Format](wad.md).

## .SWD Format

SWD format is shared with several later versions. See [SWD Format](swd.md).

