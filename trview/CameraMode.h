#pragma once

namespace trview
{
    // Camera movement.
    // Defines the behaviour of the camera.
    enum class CameraMode
    {
        // The camera is orbiting around the centre of a room.
        Orbit,
        // The camera is free roaming - the user is in control.
        Free,
        // Axis mode - the camera is free roaming but movement is constrained to the axes.
        Axis
    };
}
