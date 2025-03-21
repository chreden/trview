# Static Mesh

# Properties

| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| breakable | boolean | R | Whether this is a breakable static |
| collision | [BoundingBox](boundingbox.md) | R | Collision bounding box |
| has_collision | boolean | R | Whether this is a collidable mesh |
| id | number | R | Mesh ID or sprite texture ID |
| position | [Vector3](vector3.md) | R | Static mesh position in the world. |
| room | [Room](room.md) | R | Room that contains the static mesh |
| rotation | number | R | Static mesh rotation |
| type | string | R | Type of static mesh (Mesh/Sprite) |
| visible| boolean | RW | Whether the item is visible in the viewer |
| visibility | [BoundingBox](boundingbox.md) | R | Visibility bounding box |
