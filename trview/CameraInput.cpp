#include "stdafx.h"
#include "CameraInput.h"

namespace trview
{
    // Get the movement vector based on the current input state.
    // Returns: The movement vector.
    DirectX::SimpleMath::Vector3 CameraInput::movement() const
    {
        return DirectX::SimpleMath::Vector3(
            _free_left ? -1.0f : 0.0f + _free_right ? 1.0f : 0.0f,
            _free_up ? 1.0f : 0.0f + _free_down ? -1.0f : 0.0f,
            _free_forward ? 1.0f : 0.0f + _free_backward ? -1.0f : 0.0f);
    }
}
