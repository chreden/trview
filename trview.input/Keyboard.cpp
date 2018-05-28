#include "Keyboard.h"
#include <Windows.h>
#include <algorithm>
#include <unordered_map>
#include <vector>

namespace trview
{
    namespace input
    {
        namespace
        {
            WNDPROC old_procedure;
            std::unordered_map<HWND, std::vector<Keyboard*>> all_keyboards;

            LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
            {
                // Get the keyboard (s) for the current window.
                auto iter = all_keyboards.find(hWnd);
                if (iter != all_keyboards.end())
                {
                    const auto& kbs = iter->second;

                    switch (message)
                    {
                    case WM_KEYDOWN:
                    {
                        std::for_each(kbs.begin(), kbs.end(), [wParam](auto& k) { k->on_key_down(static_cast<uint16_t>(wParam)); });
                        break;
                    }
                    case WM_CHAR:
                    {
                        std::for_each(kbs.begin(), kbs.end(), [wParam](auto& k) { k->on_char(static_cast<uint16_t>(wParam)); });
                        break;
                    }
                    case WM_KEYUP:
                    {
                        std::for_each(kbs.begin(), kbs.end(), [wParam](auto& k) { k->on_key_up(static_cast<uint16_t>(wParam)); });
                        break;
                    }
                    }
                }

                return CallWindowProc(old_procedure, hWnd, message, wParam, lParam);
            }
        }

        Keyboard::Keyboard(HWND window)
            : _window(window)
        {
            old_procedure = (WNDPROC)GetWindowLongPtr(window, GWLP_WNDPROC);
            SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
            all_keyboards[window].push_back(this);
        }

        bool Keyboard::control() const
        {
            return GetAsyncKeyState(VK_CONTROL);
        }
    }
}
