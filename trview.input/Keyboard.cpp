#include "Keyboard.h"

namespace trview
{
    namespace input
    {
        Keyboard::Keyboard(const Window& window)
            : MessageHandler(window)
        {
        }

        Keyboard::~Keyboard()
        {
        }

        bool Keyboard::control() const
        {
            return GetKeyState(VK_CONTROL) & 0x8000;
        }

        bool Keyboard::shift() const
        {
            return GetKeyState(VK_SHIFT) & 0x8000;
        }

        void Keyboard::process_message(UINT message, WPARAM wParam, LPARAM)
        {
            bool control_pressed = control();
            bool shift_pressed = shift();
            switch (message)
            {
                case WM_KEYDOWN:
                {
                    on_key_down(static_cast<uint16_t>(wParam), control_pressed, shift_pressed);
                    break;
                }
                case WM_CHAR:
                {
                    on_char(static_cast<uint16_t>(wParam));
                    break;
                }
                case WM_KEYUP:
                {
                    on_key_up(static_cast<uint16_t>(wParam), control_pressed, shift_pressed);
                    break;
                }
            }
        }
    }
}
