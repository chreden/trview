# tr_animated_texture_range

Early version of animated textures that specifies the start and end room object texture numbers. Everything including those and in between is part of the animated sequence.

This was later abandoned in favour of a count and a sequence of texture indices, probably for flexibility of placement in the textile.

## Versions

- [Version 11](version11.md)
- [Version 21](version21.md)
- [Version 26](version26.md)

## Format

| Data Type | Description    |
| ----------| -------------  |
| `uint32`  | Start of animated texture sequence, index into object textures. |
| `uint32`  | End of animated texture sequence, index into object textures. |
