#include "Mouse.h"
#include <CommCtrl.h>

namespace trview
{
    namespace input
    {
        Mouse::Mouse(HWND window)
            : MessageHandler(window)
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

        Mouse::~Mouse()
        {
        }

        void Mouse::process_input(const RAWINPUT& input)
        {
            if (input.header.dwType == RIM_TYPEMOUSE)
            {
                auto active_window = window_under_cursor();

                if (active_window == window() && input.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
                {
                    mouse_down(Button::Left);
                }

                if (input.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
                {
                    mouse_up(Button::Left);
                }

                if (active_window == window() && input.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
                {
                    mouse_down(Button::Right);
                }

                if (input.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
                {
                    mouse_up(Button::Right);
                }

                if (input.data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
                {
                    mouse_move(input.data.mouse.lLastX, input.data.mouse.lLastY);
                }

                if (input.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
                {
                    raise_absolute_mouse_move(input.data.mouse.lLastX, input.data.mouse.lLastY);
                }

                if (input.data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
                {
                    mouse_wheel(input.data.mouse.usButtonData);
                }
            }
        }

        long Mouse::x() const
        {
            return _absolute_x;
        }

        long Mouse::y() const
        {
            return _absolute_y;
        }

        void Mouse::raise_absolute_mouse_move(long x, long y)
        {
            if (_any_absolute_previous)
            {
                mouse_move(x - _absolute_x, y - _absolute_y);
            }

            _any_absolute_previous = true;
            _absolute_x = x;
            _absolute_y = y;
        }

        void Mouse::process_message(HWND, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message)
            {
                case WM_MOUSEACTIVATE:
                {
                    const auto mouse_message = HIWORD(lParam);
                    if (mouse_message == WM_LBUTTONDOWN)
                    {
                        mouse_down(Mouse::Button::Left);
                    }
                    else if(mouse_message == WM_RBUTTONDOWN)
                    {
                        mouse_down(Mouse::Button::Right);
                    }
                    break;
                }
                case WM_MOUSEWHEEL:
                {
                    mouse_wheel(static_cast<int16_t>(GET_WHEEL_DELTA_WPARAM(wParam)));
                    break;
                }
                case WM_INPUT:
                {
                    HRAWINPUT input_handle = reinterpret_cast<HRAWINPUT>(lParam);

                    uint32_t size = 0;
                    if (GetRawInputData(input_handle, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER))
                        != static_cast<uint32_t>(-1))
                    {
                        std::vector<uint8_t> data_buffer(size);
                        GetRawInputData(input_handle, RID_INPUT, &data_buffer[0], &size, sizeof(RAWINPUTHEADER));
                        RAWINPUT& data = *reinterpret_cast<RAWINPUT*>(&data_buffer[0]);
                        process_input(data);
                    }
                    break;
                }
            }
        }
    }
}
