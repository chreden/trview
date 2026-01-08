# Version 9

This is the earliest version that TRView can understand, although the implementation is based on only one example (LEVEL3.TOM) so may be incorrect.

## Format

This format is split into three files: `.SWD`, `.TOM` and `.WAD`. The only known example for version 9 doesn't actually have the files for `.WAD` and `.SWD` but you can make a copy of the [version 11](version11.md) files and you will be able to see Lara.

The WAD and SWD files will be merged into the main file in later versions.

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
| `uint32` | Number of entities |
| [tr_entity_may_1996](tr_entity_may_1996.md)[n] | Entities |
| `uint8`[8192] | Light map |

### Floordata

Floordata is in a slightly different order to release TR1. Details are [here](floordata_9.md).

## .WAD Format

WAD format is shared with several later versions. See [WAD Format](wad.md).

## .SWD Format

SWD format is shared with several later versions. See [SWD Format](swd.md).

