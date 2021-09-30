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
Depth Selector      | Choose the depth of neighbours to show
Wireframe           | Enable wireframe rendering mode
Bounds              | Show static mesh bounding boxes
Flip                | Toggle the level flipmap (if present in the level). In TR4+ this will be a flipmap group selector.

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

Each Item can be hidden or made visible by clicking the checkbox in the 'Hide' column.

### Triggers Window

Input|Action
---|------
Track Room          | Whether to show the triggers that are in the currently selected room
Sync Item           | Whether the trigger selected in the window will be synced with the item selected in the view (bidirectional)

The column headers can be clicked to sort the items by that column. Clicking a trigger in the list will focus the camera on it in the viewer. The details of the trigger will be displayed in the Trigger Details panel.
The 'Commands' list shows the commands carried out when the trigger executes. Clicking on 'Object' or 'Look at Item' commands will select the item.

<</>>               This button collapses or expands the side panel.

Multiple Trigger windows can be active at once.

Each Trigger can be hidden or made visible by clicking the checkbox in the 'Hide' column.

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

### Angles

At the bottom left of the screen are the current camera angles. You can click on yaw or pitch and enter new camera angles. Tab will cycle from X to Y to Z and enter will confirm the change. The format of the angles can be chosen in the settings window. By default the display is in degrees but radians can be selected.

### Coordinates

At the bottom left of the screen are the current camera coordinates. You can click on X, Y, or Z and enter camera coordinates to go that location. Tab will cycle from X to Y to Z and enter will confirm the change.

## Context Menu
The context menu is opened by right clicking in the 3D view.

### Add Waypoint
Add a new route waypoint at this position. This is only available when a surface was clicked.

### Add Mid-Waypoint
Add a new route waypoint at the centre of the square that was clicked. This is only available when a floor was clicked.

### Remove Waypoint
Remove the clicked waypoint from the route. This is only available when a waypoint was clicked.

### Orbit Here
Switch the camera to orbit mode and orbit around the position that was right clicked when the context menu was opened. This is only available when a surface was clicked on.

### Hide
Hide the clicked entity or trigger. This can be unhidden from the view menu or the items or triggers window. This is only available if an item or trigger was clicked.

## Menus

### File

#### Open
Open a level file.
#### Open Recent
Open a recently opened file. Up to 10 successfully opened files will appear here.
#### Switch Level
Open another level in the same folder as the currently opened level file.
#### Exit
Close trview.

### View
#### UI
Toggle on or off visibility of all UI elements. This will override the other visibility options below if set to off.
#### Minimap
Toggle visibility of the minimap.
#### Tooltip
Toggle visibility of the tooltip when hovering over elements in the viewer.
#### Compass
Toggle visibility of the compass.
#### Selection
Toggle visibility of the selection highlights.
#### Route
Toggle visibility of the route waypoints and paths.
#### Tools
Toggle visibility of the tools, such as the measure tool.
#### Background Colour
Change the background colour for the 3D view.
#### Unhide All
Unhides all entities and triggers that have been hidden via the context menu or their windows.

### Windows
#### Items
Create a new items window.
#### Triggers
Create a new triggers window.
#### Rooms
Create a new rooms window.
#### Route
Create or ppen the route window.

### Help
#### Github
Go to the trview GitHub page. 
#### Discord
Permanent invite to the trview Discord.
#### About
Show the about box.

## Licenses

### Lua

Copyright © 1994–2020 Lua.org, PUC-Rio.
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

### boost-ext/di

https://github.com/boost-ext/di/

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

### nlohmann/json

```    
    __ _____ _____ _____
 __|  |   __|     |   | |  JSON for Modern C++
|  |  |__   |  |  | | | |  version 3.1.2
|_____|_____|_____|_|___|  https://github.com/nlohmann/json

```
Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2013-2018 Niels Lohmann <http://nlohmann.me>.

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

### zlib

zlib.h -- interface of the 'zlib' general purpose compression library
version 1.2.11, January 15th, 2017

Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

Jean-loup Gailly        Mark Adler
jloup@gzip.org          madler@alumni.caltech.edu


The data format used by the zlib library is described by RFCs (Request for
Comments) 1950 to 1952 in the files http://tools.ietf.org/html/rfc1950
(zlib format), rfc1951 (deflate format) and rfc1952 (gzip format).

### DirectXTK

                               The MIT License (MIT)

Copyright (c) 2018 Microsoft Corp

Permission is hereby granted, free of charge, to any person obtaining a copy of this 
software and associated documentation files (the "Software"), to deal in the Software 
without restriction, including without limitation the rights to use, copy, modify, 
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
permit persons to whom the Software is furnished to do so, subject to the following 
conditions: 

The above copyright notice and this permission notice shall be included in all copies 
or substantial portions of the Software.  

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

### Googletest/Googlemock

Copyright 2008, Google Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the
distribution.
    * Neither the name of Google Inc. nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
