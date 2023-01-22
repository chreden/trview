# Item

The Item library provides information about an item in a [level](level.md).

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| activation_flags | number | R | Bit flags used in combination with trigger flags |
| number | number | R | Item number |
| ocb | number | R | Object code bit, used for extra actions in TR4+ |
| position | [Vector3](vector3.md) | R | Position of the object in game units
| room | [Room](room.md) | R | The room that the item is in |
| triggered_by | [Trigger](trigger.md)[] | R | The triggers that reference this item |
| type | string | R | The item type name e.g Lara |
| type_id | number | R | The item type number |
| visible | boolean | RW | Whether the item is visible in the viewer |