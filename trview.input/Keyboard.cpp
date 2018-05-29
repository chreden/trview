#include "Keyboard.h"
#include <algorithm>

namespace trview
{
    namespace input
    {
        namespace
        {
            std::shared_ptr<Keyboard::KeyboardMap> all_keyboards;

            LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
            {
                // Get the keyboard (s) for the current window.
                auto iter = all_keyboards->find(hWnd);
                if (iter != all_keyboards->end())
                {
                    const auto& kbs = iter->second.keyboards;
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
                    return CallWindowProc(iter->second.old_procedure, hWnd, message, wParam, lParam);
                }
                return 0;
            }

            // Subclass the window if it hasn't already been done. Allows for us to get the window
            // messages and look for ones related to keyboard input. The messages are then passed
            // on to the original window procedure.
            void subclass_window(HWND window)
            {
                if ((*all_keyboards)[window].old_procedure == nullptr)
                {
                    (*all_keyboards)[window].old_procedure = (WNDPROC)GetWindowLongPtr(window, GWLP_WNDPROC);
                    SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
                }
            }

            // Add the keyboard to the keyboard map so that messages can be sent to it.
            // Returns a shared pointer to the map that the keyboard should use to interact with the map.
            std::shared_ptr<Keyboard::KeyboardMap> register_keyboard(HWND window, Keyboard* keyboard)
            {
                if (!all_keyboards)
                {
                    all_keyboards = std::make_shared<Keyboard::KeyboardMap>();
                }
                subclass_window(window);
                (*all_keyboards)[window].keyboards.push_back(keyboard);
                return all_keyboards;
            }
        }

        Keyboard::Keyboard(HWND window)
            : _window(window)
        {
            _all_keyboards = register_keyboard(window, this);
        }

        Keyboard::~Keyboard()
        {
            // Remove the keyboard from the collection so the window procedure doesn't try
            // to keep sending messages to it after it has been destroyed.
            auto& kbs = (*_all_keyboards)[_window].keyboards;
            kbs.erase(std::remove(kbs.begin(), kbs.end(), this), kbs.end());
        }

        bool Keyboard::control() const
        {
            return GetAsyncKeyState(VK_CONTROL);
        }
    }
}
