#include "stdafx.h"
#include "CameraInput.h"
#include "CameraMode.h"

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

    // Process a key being pressed.
    // key: The key that was pressed.
    void CameraInput::key_down(uint16_t key)
    {
        switch (key)
        {
            case 'F':
            {
                on_mode_change(CameraMode::Free);
                break;
            }
            case 'O':
            {
                on_mode_change(CameraMode::Orbit);
                break;
            }
            case 'X':
            {
                on_mode_change(CameraMode::Axis);
                break;
            }
            case 'Q':
            {
                _free_down = true;
                break;
            }
            case 'E':
            {
                _free_up = true;
                break;
            }
            case 'W':
            {
                _free_forward = true;
                break;
            }
            case 'A':
            {
                _free_left = true;
                break;
            }
            case 'D':
            {
                _free_right = true;
                break;
            }
            case 'S':
            {
                _free_backward = true;
                break;
            }
        }
    }

    // Process a key being released.
    // key: The key that was released.
    void CameraInput::key_up(uint16_t key)
    {
        switch (key)
        {
            case 'Q':
            {
                _free_down = false;
                break;
            }
            case 'E':
            {
                _free_up = false;
                break;
            }
            case 'W':
            {
                _free_forward = false;
                break;
            }
            case 'A':
            {
                _free_left = false;
                break;
            }
            case 'D':
            {
                _free_right = false;
                break;
            }
            case 'S':
            {
                _free_backward = false;
                break;
            }
        }
    }

    // Process a mouse button being pressed.
    // button: The button that was pressed.
    void CameraInput::mouse_down(input::Mouse::Button button)
    {
        if (button == input::Mouse::Button::Right)
        {
            _rotating = true;
        }
    }

    // Process a mouse button being released.
    // button: The button that was released.
    void CameraInput::mouse_up(input::Mouse::Button button)
    {
        if (button == input::Mouse::Button::Right)
        {
            _rotating = false;
        }
    }

    // Process the mouse being moved.
    // x: The x movement.
    // y: The y movement.
    void CameraInput::mouse_move(long x, long y)
    {
        if (_rotating)
        {
            const float low_sensitivity = 200.0f;
            const float high_sensitivity = 25.0f;
            const float sensitivity = low_sensitivity + (high_sensitivity - low_sensitivity) * _camera_sensitivity;
            on_rotate(x / sensitivity, y / sensitivity);
        }
    }

    // Process the mouse wheel being turned.
    // scroll: The mouse wheel movement.
    void CameraInput::mouse_scroll(int16_t scroll)
    {
        on_zoom(scroll / -100.0f);
    }
}
