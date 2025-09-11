# Item

The Item library provides information about an item in a [level](level.md).

# Properties
| Name | Type | Mode | Description |
| ---- | ---- | ---- | ---- |
| activation_flags | number | R | Bit flags used in combination with trigger flags |
| ai | boolean | R | Whether this is an AI object (only potentially true in TR4+) |
| angle | number | R | Y rotation in range 0-65535 |
| categories | string[] | RW | Categories that the item belongs to. Initially inherited by trview type list but can be overwrritten |
| clear_body | boolean | R | Flag used together with Clear Bodies trigger action to remove the body of dead enemy from the level to conserve resources |
| invisible | boolean | R | Flag that determines whether the item is invisible in-game or not |
| ng | boolean | R | Whether this is an NG+ item. `nil` for item not involved in NG+, `false` for item that will be switched, `true` for switched item |
| number | number | R | Item number |
| ocb | number | R | Object code bit, used for extra actions in TR4+ |
| position | [Vector3](vector3.md) | R | Position of the object in game units
| remastered_extra | boolean | R | Whether this is an extra item generated at runtime by the remasters |
| room | [Room](room.md) | R | The room that the item is in |
| trigger_references | [Trigger](trigger.md)[] | R | The triggers that reference this item |
| type | string | R | The item type name e.g Lara |
| type_id | number | R | The item type number |
| visible | boolean | RW | Whether the item is visible in the viewer |