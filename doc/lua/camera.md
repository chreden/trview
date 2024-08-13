# Camera

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| mode | string | RW | One of `free`, `orbit`, `axis` |
| target | [Vector3](vector3.md) | RW | Target position in the world. When writing can be a [Vector3](vector3.md) or anything with a `position` field, such as [Item](item.md), [Trigger](trigger.md) and others, or any table. |