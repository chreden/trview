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

        // Subclass the window if it hasn't already been done. Allows for us to get the window
        // messages and look for ones related to keyboard input. The messages are then passed
        // on to the original window procedure.
        void subclass_window(HWND window)
        {
            if (old_procedure == nullptr)
            {
                old_procedure = (WNDPROC)GetWindowLongPtr(window, GWLP_WNDPROC);
                SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
            }
        }

        Keyboard::Keyboard(HWND window)
            : _window(window)
        {
            subclass_window(window);
            all_keyboards[window].push_back(this);
        }

        Keyboard::~Keyboard()
        {
            // Remove the keyboard from the collection so the window procedure doesn't try
            // to keep sending messages to it after it has been destroyed.
            auto& kbs = all_keyboards[_window];
            kbs.erase(std::remove(kbs.begin(), kbs.end(), this), kbs.end());
        }

        bool Keyboard::control() const
        {
            return GetAsyncKeyState(VK_CONTROL);
        }
    }
}
