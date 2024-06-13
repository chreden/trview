# Triggers Window
_Shortcut: Ctrl+T_

Shows the list of triggers that exist in the current level and allows you to filter, sort, inspect and add triggers to a route.

![Triggers Window](triggers.png)

## Options

Input|Action
---|------
Filters | Open the [filters](filters.md) editor. The checkbox toggles whether configured filters are applied.
Track Room          | Whether to show the triggers that are in the currently selected room. Clicking the button will show the expanded track menu where you can choose whether rooms are tracked.
Sync | Whether the trigger selected in the window will be synced with the trigger selected in the view (bidirectional)

## Triggers List

Column | Meaning
---|---
\# | The trigger number
Room | The number of the room that the trigger is in
Type | The type of the trigger
Hide | Whether to hide this trigger in the viewer

## Trigger Details

Stat | Meaning
--- | ---
Type | The type of the trigger
\# | The trigger number
Position | The position of the trigger in world space
Room | The number of the room that the trigger is in
Flags | Activation mask
Only Once | Whether this trigger can only be triggered once
Timer | Used to time trigger activation/deactivation
Colour | The colour of the trigger in the 3D view. Can be reset with the `Reset` button.

## Add To Route
Clicking the `Add to Route` button will insert this trigger at the current position in the route.

## Commands
The `Commands` list shows the commands that will be executed when the trigger is activated. Clicking commands that reference an `Item` will select this `item` in the viewer.

Column | Meaning
---|---
Type | The type name of the trigger
Index | The numerical value for this trigger - this could be an item number or some other value
Entity | The name of the entity referenced if appropriate