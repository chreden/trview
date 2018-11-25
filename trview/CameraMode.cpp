#include "stdafx.h"
#include "CameraMode.h"

namespace trview
{
    FreeCamera::Alignment camera_mode_to_alignment(CameraMode mode)
    {
        return mode == CameraMode::Axis ? FreeCamera::Alignment::Axis : FreeCamera::Alignment::Camera;
    }
}