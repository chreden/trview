#include "CameraInput.h"

namespace trview
{
    DirectX::SimpleMath::Vector3 CameraInput::movement() const
    {
        return DirectX::SimpleMath::Vector3(
            _free_left ? -1.0f : 0.0f + _free_right ? 1.0f : 0.0f,
            _free_up ? -1.0f : 0.0f + _free_down ? 1.0f : 0.0f,
            _free_forward ? 1.0f : 0.0f + _free_backward ? -1.0f : 0.0f);
    }

    void CameraInput::key_down(uint16_t key, bool control)
    {
        if (control)
        {
            return;
        }

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

    void CameraInput::mouse_down(input::Mouse::Button button)
    {
        if (button == input::Mouse::Button::Right)
        {
            _rotating = true;
        }
        else if (button == input::Mouse::Button::Left)
        {
            _panning = true;
        }
    }

    void CameraInput::mouse_up(input::Mouse::Button button)
    {
        if (button == input::Mouse::Button::Right)
        {
            _rotating = false;
        }
        else if (button == input::Mouse::Button::Left)
        {
            _panning = false;
        }
    }

    void CameraInput::mouse_move(long x, long y)
    {
        if (_rotating)
        {
            on_rotate(static_cast<float>(x), static_cast<float>(y));
        }

        if (_panning)
        {
            on_pan(static_cast<float>(x), static_cast<float>(y));
        }
    }

    void CameraInput::mouse_scroll(int16_t scroll)
    {
        on_zoom(scroll / -100.0f);
    }
}
