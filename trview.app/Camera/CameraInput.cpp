#include "CameraInput.h"

namespace trview
{
    namespace
    {
        constexpr uint16_t Scan_Q = 0x0010;
        constexpr uint16_t Scan_W = 0x0011;
        constexpr uint16_t Scan_E = 0x0012;
        constexpr uint16_t Scan_O = 0x0018;
        constexpr uint16_t Scan_A = 0x001E;
        constexpr uint16_t Scan_S = 0x001F;
        constexpr uint16_t Scan_D = 0x0020;
        constexpr uint16_t Scan_F = 0x0021;
        constexpr uint16_t Scan_X = 0x002D;
    }

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
            case Scan_F:
            {
                on_mode_change(ICamera::Mode::Free);
                break;
            }
            case Scan_O:
            {
                on_mode_change(ICamera::Mode::Orbit);
                break;
            }
            case Scan_X:
            {
                on_mode_change(ICamera::Mode::Axis);
                break;
            }
            case Scan_Q:
            {
                _free_down = true;
                break;
            }
            case Scan_E:
            {
                _free_up = true;
                break;
            }
            case Scan_W:
            {
                _free_forward = true;
                break;
            }
            case Scan_A:
            {
                _free_left = true;
                break;
            }
            case Scan_D:
            {
                _free_right = true;
                break;
            }
            case Scan_S:
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
            case Scan_Q:
            {
                _free_down = false;
                break;
            }
            case Scan_E:
            {
                _free_up = false;
                break;
            }
            case Scan_W:
            {
                _free_forward = false;
                break;
            }
            case Scan_A:
            {
                _free_left = false;
                break;
            }
            case Scan_D:
            {
                _free_right = false;
                break;
            }
            case Scan_S:
            {
                _free_backward = false;
                break;
            }
        }
    }

    void CameraInput::mouse_down(input::IMouse::Button button)
    {
        if (button == input::IMouse::Button::Right)
        {
            _rotating = true;
            _panning_vertical = _panning;
        }
        else if (button == input::IMouse::Button::Left)
        {
            _panning = true;
            _panning_vertical = _rotating;
        }
    }

    void CameraInput::mouse_up(input::IMouse::Button button)
    {
        if (button == input::IMouse::Button::Right)
        {
            _rotating = false;
            _panning_vertical = false;
        }
        else if (button == input::IMouse::Button::Left)
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

    void CameraInput::reset(bool ignore_key_states)
    {
        _free_forward = ignore_key_states ? false : GetAsyncKeyState(MapVirtualKey(Scan_W, MAPVK_VSC_TO_VK)) & 0x8000;
        _free_left = ignore_key_states ? false : GetAsyncKeyState(MapVirtualKey(Scan_A, MAPVK_VSC_TO_VK)) & 0x8000;
        _free_right = ignore_key_states ? false : GetAsyncKeyState(MapVirtualKey(Scan_D, MAPVK_VSC_TO_VK)) & 0x8000;
        _free_backward = ignore_key_states ? false : GetAsyncKeyState(MapVirtualKey(Scan_S, MAPVK_VSC_TO_VK)) & 0x8000;
        _free_up = ignore_key_states ? false : GetAsyncKeyState(MapVirtualKey(Scan_E, MAPVK_VSC_TO_VK)) & 0x8000;
        _free_down = ignore_key_states ? false : GetAsyncKeyState(MapVirtualKey(Scan_Q, MAPVK_VSC_TO_VK)) & 0x8000;
        _rotating = false;
        _panning = false;
        _panning_vertical = false;
    }

    void CameraInput::reset_input()
    {
        _free_forward = false;
        _free_left = false;
        _free_right = false;
        _free_backward = false;
        _free_up = false;
        _free_down = false;
    }
}
