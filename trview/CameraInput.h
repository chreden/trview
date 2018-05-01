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
        // Process a key being pressed down.
        // key: The key that was pressed.
        void on_key_down(uint16_t key);
        // Process a key being released.
        // key: The key that was released.
        void on_key_up(uint16_t key);
    private:
        bool _free_forward{ false };
        bool _free_left{ false };
        bool _free_right{ false };
        bool _free_backward{ false };
        bool _free_up{ false };
        bool _free_down{ false };
    };
}