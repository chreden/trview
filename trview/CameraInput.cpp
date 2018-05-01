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

    void CameraInput::on_key_down(uint16_t key)
    {
        switch (key)
        {
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

    void CameraInput::on_key_up(uint16_t key)
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
}
