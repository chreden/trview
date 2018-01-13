#include "Mouse.h"
#include <Windows.h>

namespace trview
{
    namespace input
    {
        Mouse::Mouse()
        {
            // Register raw input devices so that the window
            // will receive the raw input messages.
            RAWINPUTDEVICE devices[1];
            memset(devices, 0, sizeof(devices));

            devices[0].usUsagePage = 0x01;
            devices[0].usUsage = 0x02;
            devices[0].dwFlags = 0;
            devices[0].hwndTarget = 0;

            RegisterRawInputDevices(devices, sizeof(devices) / sizeof(RAWINPUTDEVICE), sizeof(RAWINPUTDEVICE));
        }

        void Mouse::process_input(const RAWINPUT& input)
        {
            if (input.header.dwType == RIM_TYPEMOUSE)
            {
                if (input.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
                {
                    raise_mouse_down(Button::Left);
                }

                if (input.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
                {
                    raise_mouse_up(Button::Left);
                }

                if (input.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
                {
                    raise_mouse_down(Button::Right);
                }

                if (input.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
                {
                    raise_mouse_up(Button::Right);
                }

                if (input.data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
                {
                    raise_relative_mouse_move(input.data.mouse.lLastX, input.data.mouse.lLastY);
                }

                if (input.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
                {
                    raise_absolute_mouse_move(input.data.mouse.lLastX, input.data.mouse.lLastY);
                }

                if (input.data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
                {
                    raise_mouse_wheel(input.data.mouse.usButtonData);
                }
            }
        }

        void Mouse::raise_mouse_down(Button button)
        {
            mouse_down.raise(button);
        }

        void Mouse::raise_mouse_up(Button button)
        {
            mouse_up.raise(button);
        }

        void Mouse::raise_mouse_wheel(short delta)
        {
            mouse_wheel.raise(delta);
        }

        void Mouse::raise_relative_mouse_move(long x, long y)
        {
            mouse_move.raise(x, y);
        }

        void Mouse::raise_absolute_mouse_move(long x, long y)
        {
            if (_any_absolute_previous)
            {
                raise_relative_mouse_move(x - _absolute_x, y - _absolute_y);
            }

            _any_absolute_previous = true;
            _absolute_x = x;
            _absolute_y = y;
        }
    }
}
