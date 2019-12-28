#include "Mouse.h"
#include <CommCtrl.h>

namespace trview
{
    namespace input
    {
        namespace
        {
            /// The amount of milliseconds before a click is no longer considered a click.
            const uint32_t ClickDelta = 200;
        }

        Mouse::Mouse(const Window& window, std::unique_ptr<IWindowTester>&& window_tester)
            : MessageHandler(window), _window_tester(std::move(window_tester))
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

        void Mouse::update_button(Button button, USHORT flags, USHORT down_mask, USHORT up_mask, DWORD& last_down)
        {
            if (_window_tester->is_window_under_cursor() && flags & down_mask)
            {
                last_down = GetTickCount();
                mouse_down(button);
            }

            if (flags & up_mask)
            {
                auto difference = GetTickCount() - last_down;
                if (difference < ClickDelta)
                {
                    mouse_click(button);
                }
                mouse_up(button);
            }
        }

        void Mouse::process_input(const RAWINPUT& input)
        {
            if (input.header.dwType == RIM_TYPEMOUSE)
            {
                update_button(Button::Left, input.data.mouse.usButtonFlags, RI_MOUSE_LEFT_BUTTON_DOWN, RI_MOUSE_LEFT_BUTTON_UP, _left_down);
                update_button(Button::Middle, input.data.mouse.usButtonFlags, RI_MOUSE_MIDDLE_BUTTON_DOWN, RI_MOUSE_MIDDLE_BUTTON_UP, _middle_down);
                update_button(Button::Right, input.data.mouse.usButtonFlags, RI_MOUSE_RIGHT_BUTTON_DOWN, RI_MOUSE_RIGHT_BUTTON_UP, _right_down);

                if (input.data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
                {
                    mouse_move(input.data.mouse.lLastX, input.data.mouse.lLastY);
                }

                if (input.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
                {
                    bool is_rdp = input.data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP;
                    raise_absolute_mouse_move(
                        static_cast<long>((input.data.mouse.lLastX / static_cast<float>(USHRT_MAX)) * _window_tester->screen_width(is_rdp)),
                        static_cast<long>((input.data.mouse.lLastY / static_cast<float>(USHRT_MAX)) * _window_tester->screen_height(is_rdp)));
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

        void Mouse::process_message(UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message)
            {
                case WM_MOUSEACTIVATE:
                {
                    const auto mouse_message = HIWORD(lParam);
                    if (mouse_message == WM_LBUTTONDOWN)
                    {
                        mouse_down(Button::Left);
                    }
                    else if (mouse_message == WM_MBUTTONDOWN)
                    {
                        mouse_down(Button::Middle);
                    }
                    else if(mouse_message == WM_RBUTTONDOWN)
                    {
                        mouse_down(Button::Right);
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
