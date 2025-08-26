# Version 11

This is the earliest version that TRView can understand with the exception of one version 9 level.

## Format

This format is split into three files: `.SWD`, `.TOM` and `.WAD`.

The WAD and SWD files will be merged into the main file in later versions.

## Changes from Version 9

- Unknown `uint32` added, most likely camera count.
- Boxes added
- Overlaps added
- Animated textures added

## .TOM Format

| Data Type | Description    |
| ----------| -------------  |
| `uint32`  | Version number |
| `uint16`  | Room texture count |
| [tr_object_texture_may_1996](tr_object_texture_may_1996.md)[n] | Room object textures |
| `uint32` | Textile size in bytes |
| `uint8`[n] | Textile bytes |
| [tr_colour](tr_colour.md)[256] | 8-bit palette |
| `uint16` | Room count |
| [tr_pc_room_may_1996](tr_pc_room_may_1996.md)[n] | Rooms |
| `uint32` | Floordata length |
| `uint16` | Floordata |
| `uint32` | Unknown. Possibly camera count. |
| `uint32` | Number of boxes |
| [tr_box](tr_box.md)[n] | Boxes |
| `uint32` | Number of overlaps |
| `uint16`[n] | Overlaps |
| `uint32` | Number of animated texture ranges |
| [tr_animated_texture_range](tr_animated_texture_range.md) | Animated texture ranges |
| `uint32` | Number of entities |
| [tr_entity_may_1996](tr_entity_may_1996.md)[n] | Entities |
| `uint8`[8192] | Light map |

## .WAD Format

WAD format is shared with several later versions. See [WAD Format](wad.md).

## .SWD Format

SWD format is shared with several later versions. See [SWD Format](swd.md).

