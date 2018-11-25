/// @file CameraMode.h
/// @brief Defines which type of camera to use. The different cameras have different behaviours.
/// 
/// Defines the different camera modes. The camera will behave differently depending on the selected
/// camera mode.

#pragma once

#include <trview.app/FreeCamera.h>

namespace trview
{
    /// Defines the behaviour of the camera and the way it reacts to input.
    enum class CameraMode
    {
        /// The camera is orbiting around the centre of a room.
        Orbit,
        /// The camera is free roaming - the user is in control and can freely rotate on the spot
        /// and can move in any direction.
        Free,
        /// The camera is free roaming but movement is constrained to the axes.
        Axis
    };

    FreeCamera::Alignment camera_mode_to_alignment(CameraMode mode);
}
