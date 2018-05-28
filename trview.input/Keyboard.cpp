#include "Keyboard.h"
#include <Windows.h>
#include <algorithm>

namespace trview
{
    namespace input
    {
        namespace
        {
            WNDPROC old_procedure;

            LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
            {
                switch (message)
                {
                }

                return CallWindowProc(old_procedure, hWnd, message, wParam, lParam);
            }
        }

        Keyboard::Keyboard(HWND window)
            : _window(window)
        {
            old_procedure = (WNDPROC)GetWindowLongPtr(window, GWLP_WNDPROC);
            SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
        }

        bool Keyboard::control() const
        {
            return GetAsyncKeyState(VK_CONTROL);
        }
    }
}
