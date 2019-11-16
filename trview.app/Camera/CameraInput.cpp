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
            _panning_vertical = _panning;
        }
        else if (button == input::Mouse::Button::Left)
        {
            _panning = true;
            _panning_vertical = _rotating;
        }
    }

    void CameraInput::mouse_up(input::Mouse::Button button)
    {
        if (button == input::Mouse::Button::Right)
        {
            _rotating = false;
            _panning_vertical = false;
        }
        else if (button == input::Mouse::Button::Left)
        {
            _panning = false;
            _panning_vertical = false;
        }
    }

    void CameraInput::mouse_move(long x, long y)
    {
        if (_rotating && !_panning_vertical)
        {
            on_rotate(static_cast<float>(x), static_cast<float>(y));
        }

        if (_panning)
        {
            on_pan(_panning_vertical, static_cast<float>(x), static_cast<float>(y));
        }
    }

    void CameraInput::mouse_scroll(int16_t scroll)
    {
        on_zoom(scroll / -100.0f);
    }

    void CameraInput::reset()
    {
        _free_forward = GetAsyncKeyState('W') & 0x8000;
        _free_left = GetAsyncKeyState('A') & 0x8000;
        _free_right = GetAsyncKeyState('D') & 0x8000;
        _free_backward = GetAsyncKeyState('S') & 0x8000;
        _free_up = GetAsyncKeyState('E') & 0x8000;
        _free_down = GetAsyncKeyState('Q') & 0x8000;
        _rotating = false;
        _panning = false;
        _panning_vertical = false;
    }
}
