#include "Keyboard.h"

namespace trview
{
    namespace input
    {
        namespace
        {
            const int command_copy = 33000;
            const int command_paste = 33001;
            const int command_cut = 33002;
        }

        Keyboard::Keyboard(const Window& window)
            : MessageHandler(window)
        {
            // Register the copy/paste commands.
            ACCEL shortcuts[] =
            {
                { FCONTROL | FVIRTKEY, 'C', command_copy },
                { FCONTROL | FVIRTKEY, 'V', command_paste },
                { FCONTROL | FVIRTKEY, 'X', command_cut },
            };
            _accelerators = CreateAcceleratorTable(shortcuts, sizeof(shortcuts) / sizeof(ACCEL));
        }

        Keyboard::~Keyboard()
        {
        }

        bool Keyboard::control() const
        {
            return GetAsyncKeyState(VK_CONTROL) & 0x8000;
        }

        void Keyboard::process_message(UINT message, WPARAM wParam, LPARAM lParam)
        {
            bool control_pressed = control();

            MSG msg{ window(), message, wParam, lParam, GetTickCount(), 0 };
            if (TranslateAccelerator(window(), _accelerators, &msg))
            {
                return;
            }

            switch (message)
            {
                case WM_COMMAND:
                {
                    UINT id = LOWORD(wParam);
                    switch (id)
                    {
                        case command_copy:
                        {
                            return;
                        }
                        case command_paste:
                        {
                            OpenClipboard(window());
                            HANDLE data = GetClipboardData(CF_UNICODETEXT);
                            std::wstring text(static_cast<wchar_t*>(data));
                            CloseClipboard();
                            on_paste(text);
                            return;
                        }
                        case command_cut:
                        {
                            return;
                        }
                    }
                    break;
                }
                case WM_KEYDOWN:
                {
                    on_key_down(static_cast<uint16_t>(wParam), control_pressed);
                    break;
                }
                case WM_CHAR:
                {
                    on_char(static_cast<uint16_t>(wParam));
                    break;
                }
                case WM_KEYUP:
                {
                    on_key_up(static_cast<uint16_t>(wParam), control_pressed);
                    break;
                }
            }
        }
    }
}
