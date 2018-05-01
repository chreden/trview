#pragma once

#include <SimpleMath.h>
#include <cstdint>

#include <trview.input/Mouse.h>
#include <trview.common/Event.h>

namespace trview
{
    class CameraInput
    {
    public:
        // Get the movement vector based on the current input state.
        // Returns: The movement vector.
        DirectX::SimpleMath::Vector3 movement() const;
        // Process a key being pressed.
        // key: The key that was pressed.
        void on_key_down(uint16_t key);
        // Process a key being released.
        // key: The key that was released.
        void on_key_up(uint16_t key);
        // Process a mouse button being pressed.
        // button: The button that was pressed.
        void on_mouse_down(input::Mouse::Button button);
        // Process a mouse button being released.
        // button: The button that was released.
        void on_mouse_up(input::Mouse::Button button);
        // Process the mouse being moved.
        // x: The x movement.
        // y: The y movement.
        void on_mouse_move(long x, long y);
        // Event raised when the camera needs to be rotated.
        Event<float, float> on_rotate;
    private:
        bool _free_forward{ false };
        bool _free_left{ false };
        bool _free_right{ false };
        bool _free_backward{ false };
        bool _free_up{ false };
        bool _free_down{ false };
        bool _rotating{ false };
        float _camera_sensitivity{ 0.0f };
    };
}