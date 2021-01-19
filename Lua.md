# Lua API

## Contents
1. [register](#register)
2. [trview](#trview)
3. [camera](#camera)

## register
Functions to register code to run whenever some *event* happens.

### register.onrender(function)

Register some code to run whenever the application renders a new frame. This has the effect of running code every tick.

Example:

<pre>
-- rotate the camera around the target
register.onrender(function()
    if camera.mode ~= "orbit" then camera.mode = "orbit" end
    camera.yaw = camera.yaw + 0.01
end)</pre>

## trview
Functions to manipulate the trview application.

### trview.open (string)
Opens the provided path as a new level.

### trview.recent (int)
Opens one of the recent loaded files. The index provided is 1-based, so 1 is the most recent file.

### trview.flip
Gets or sets the current flip status of the loaded level. You can use `trview.flip = true` to enable flip, for example.

### trview.currentroom
Gets or sets the current room in the loaded level.

## camera

### camera.mode 
Gets or sets the current camera mode. Valid values for this are *orbit*, *free* and *axis*.

### camera.yaw
Gets or sets the rotational *yaw* of the current camera. 

### camera.pitch 
Gets or sets the rotational *pitch* of the current camera.

### camera.x, camera.y, camera.z
Gets or sets the position of the current camera. If the camera's current mode (`camera.mode`) is *orbit*, this instead changes the *camera target*.

