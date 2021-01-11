# trview

trview is a level visualiser for Tomb Raider 1 - 5 made with speedrunning 
in mind. This program is useful to quickly check how the rooms are laid out,
see the triggers present on them and analyse route possibilities.

![Screenshot](https://i.imgur.com/DuSKfC9.png)

## License

This program is licensed under the MIT license. Please read the LICENSE text
document included with the source code if you would like to read the terms of 
the license. The license can also be found online at https://opensource.org/licenses/MIT.

## Download

Source code archives and Windows binaries are available from the trview
Releases section on GitHub:

https://github.com/chreden/trview/releases

## Building

Open the project file (.sln) with Visual Studio Community 2017 and click on
Build > Build Solution.

To run the tests you will need to install the 'Test Adapter for Google Test' - it can be found in Individual Components in the Visual Studio Installer.

## Running

Double click a level file (.TR2, .TR4, .TRC or .PHD) present in the game's data folder 
and choose to open it with trview.exe. You can also open trview by itself and choose
a level file using the File menu or drag and drop a level file onto the window.

## Controls

### General

Key|Action
---|------
CTRL + O    | Open file
CTRL + G    | Show 'go to room' box
CTRL + E    | Show 'go to item' box
CTRL + R    | Open Route window
CTRL + T    | New Triggers window
CTRL + I    | New Items window
CTRL + M    | New Rooms window
F1          | Toggle settings window
H           | Toggle room highlighting
G           | Toggle hidden geometry
INSERT      | Reset camera
F           | Free camera
O           | Orbit camera
X           | Axis camera
P           | Toggle flipmap
T           | Toggle trigger visibility
Left mouse  | Click on a room to go to that room
LEFT ARROW  | Go to previous waypoint in route
RIGHT ARROW | Go to next waypoint in route
DELETE      | Delete the currently selected waypoint
RIGHT MOUSE | Show the context menu

### Free View

Key|Action
---|------
W                       | Move forward
A                       | Move left
S                       | Move backwards
D                       | Move right
E                       | Move up
Q                       | Move down
Right mouse + move      | Change look direction
Mouse wheel             | Zoom in/out

### Axis View

Key|Action
---|------
W                       | Move forward along Z axis
S                       | Move backwards along Z axis
A                       | Move backwards along X axis
D                       | Move forward along X axis
E                       | Move up
Q                       | Move down
Right mouse + move      | Change look direction

### Orbit View

Key|Action
---|------
Left mouse + move         | Pan camera
Right mouse + move        | Rotate around target
Left + Right mouse + move | Vertical panning
Mouse wheel               | Zoom in/out

### View Options

Input|Action
---|------
Highlight           | Highlight the currently selected room
Triggers            | Toggle trigger visibility
Geometry            | Toggle hidden geometry visibility
Water               | Toggle water in water rooms
Depth               | Toggle depth mode. This will show the currently selected depth of neighbours of the current room.
Depth Selector      | Choose the depth of neighbours to show.
Flip                | Toggle the level flipmap (if present in the level)

### Room

Room Selector       | Go up/down through room numbers
X                   | X coordinate of the room
Z                   | Z coordinate of the room

### Camera

Input|Action
---|------
Reset               | Reset the orbit camera
Orbit               | Use orbit mode - this orbits a room or item
Free                | Use free mode
Axis                | Use axis mode - this is like free, but the controls map to X/Z/Y axis (rotation is ignored)

## Windows

### Go To Room
Enter the room number - press enter to go to the room, escape to cancel.

### Go To Item
Enter the item number - press enter to go to the item, escape to cancel.

### Items Window

Input|Action
---|------
Track Room          | Whether to show the items that are in the currently selected room
Sync Item           | Whether the item selected in the window will be synced with the item selected in the view (bidirectional)

The column headers can be clicked to sort the items by that column. Clicking an item in the list will focus the camera on it in the viewer. The details of the item will be displayed in the Item Details panel.
The 'Triggered By' list shows the triggers that can trigger the item. Clicking a trigger in the list will select that trigger.

<</>>               This button collapses or expands the side panel.



### Triggers Window

Input|Action
---|------
Track Room          | Whether to show the triggers that are in the currently selected room
Sync Item           | Whether the trigger selected in the window will be synced with the item selected in the view (bidirectional)

The column headers can be clicked to sort the items by that column. Clicking a trigger in the list will focus the camera on it in the viewer. The details of the trigger will be displayed in the Trigger Details panel.
The 'Commands' list shows the commands carried out when the trigger executes. Clicking on 'Object' or 'Look at Item' commands will select the item.

<</>>               This button collapses or expands the side panel.

Multiple Trigger windows can be active at once.

### Rooms Window

Input|Action
---|------
Sync Room           | Whether the room selected in the window will be synced with the room selected in the view (bidirectional)
Track Item          | Whether to show the items that are selected in the currently selected room.
Track Triggers      | Whether to show the items that are selected in the currently selected room.

Multiple Room windows can be active at once.

### Route Window

Input|Action
---|------
Import              | Import a route from a file.
Export              | Export a route to a file.

The selected waypoint will have its details shown in the right panel. The notes for the route can be edited by typing in the 'Notes' text area.

### Minimap

Key|Action
---|------
Left click          | If a trigger square, will select the trigger. If not a trigger (or control is pressed) it will follow a portal, or go to the room below. 
Right click         | Goes to the room above, if one is present.

## Tools

### Measure

The measure tool can be used to measure a distance: 
* Click on the measure button at the bottom of the 3D view or press the M key 
* Click on a point in the world
* Click on a second point
A path will appear between the two points with a distance label. This is the distance in units of 1024.

### Compass

At the bottom right of the screen is the 3D compass. This shows the current direction of each axis, relative to the current camera view. Each of the nodes at the end of the axis lines can be clicked on to align the camera to that direction.

### Coordinates

At the bottom left of the screen are the current camera coordinates. You can click on X, Y, or Z and enter camera coordinates to go that location. Tab will cycle from X to Y to Z and enter will confirm the change.

## Licenses

### Lua

Copyright © 1994–2020 Lua.org, PUC-Rio.
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.