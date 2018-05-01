#pragma once

#include <SimpleMath.h>

namespace trview
{
    class CameraInput
    {
    public:
        // Get the movement vector based on the current input state.
        // Returns: The movement vector.
        DirectX::SimpleMath::Vector3 movement() const;
    private:
        bool _free_forward{ false };
        bool _free_left{ false };
        bool _free_right{ false };
        bool _free_backward{ false };
        bool _free_up{ false };
        bool _free_down{ false };
    };
}