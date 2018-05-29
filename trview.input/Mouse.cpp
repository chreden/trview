#include "Mouse.h"

namespace trview
{
    namespace input
    {
        namespace
        {
            std::shared_ptr<Mouse::MouseMap> all_mice;

            LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
            {
                // Get the mice for the current window.
                auto iter = all_mice->find(hWnd);
                if (iter != all_mice->end())
                {
                    const auto& mice = iter->second.mice;
                    switch (message)
                    {
                        case WM_MOUSEWHEEL:
                        {
                            std::for_each(mice.begin(), mice.end(), [wParam](auto& m) { m->mouse_wheel(static_cast<int16_t>(GET_WHEEL_DELTA_WPARAM(wParam))); });
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
                                std::for_each(mice.begin(), mice.end(), [data](auto& m) { m->process_input(data); });
                            }
                            break;
                        }
                    }
                    return CallWindowProc(iter->second.old_procedure, hWnd, message, wParam, lParam);
                }
                return 0;
            }

            // Subclass the window if it hasn't already been done. Allows for us to get the window
            // messages and look for ones related to keyboard input. The messages are then passed
            // on to the original window procedure.
            void subclass_window(HWND window)
            {
                if ((*all_mice)[window].old_procedure == nullptr)
                {
                    (*all_mice)[window].old_procedure = (WNDPROC)GetWindowLongPtr(window, GWLP_WNDPROC);
                    SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
                }
            }

            // Add the mouse to the mouse map so that messages can be sent to it.
            // Returns a shared pointer to the map that the mouse should use to interact with the map.
            std::shared_ptr<Mouse::MouseMap> register_mouse(HWND window, Mouse* mouse)
            {
                if (!all_mice)
                {
                    all_mice = std::make_shared<Mouse::MouseMap>();
                }
                subclass_window(window);
                (*all_mice)[window].mice.push_back(mouse);
                return all_mice;
            }
        }

        Mouse::Mouse(HWND window)
            : _window(window)
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

            _all_mice = register_mouse(window, this);
        }

        Mouse::~Mouse()
        {
            // Remove the mouse from the collection so the window procedure doesn't try
            // to keep sending messages to it after it has been destroyed.
            auto x = _all_mice->find(_window);
            auto& mice = (*_all_mice)[_window].mice;
            mice.erase(std::remove(mice.begin(), mice.end(), this), mice.end());
        }

        void Mouse::process_input(const RAWINPUT& input)
        {
            if (input.header.dwType == RIM_TYPEMOUSE)
            {
                if (input.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
                {
                    mouse_down(Button::Left);
                }

                if (input.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
                {
                    mouse_up(Button::Left);
                }

                if (input.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
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
    }
}
