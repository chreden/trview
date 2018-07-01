#include "stdafx.h"
#include "MessageHandler.h"
#include <CommCtrl.h>
#include <random>

namespace trview
{
    namespace
    {
        // Window procedure - will forward messages to the handler in the reference data.
        LRESULT CALLBACK HandlerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
        {
            MessageHandler* handler = reinterpret_cast<MessageHandler*>(dwRefData);
            if (handler)
            {
                handler->process_message(hWnd, message, wParam, lParam);
            }
            return DefSubclassProc(hWnd, message, wParam, lParam);
        }
    }

    MessageHandler::MessageHandler(HWND window)
        : _window(window)
    {
        // Try to subclass the window with a unique ID - this has some issues in that something
        // could sneak in after we have done the check, but I think it is better than nothing.
        std::default_random_engine random;
        _id = random();
        DWORD_PTR existing = 0;
        while (GetWindowSubclass(window, HandlerProc, _id, &existing))
        {
            _id = random();
        }
        SetWindowSubclass(window, HandlerProc, _id, reinterpret_cast<DWORD_PTR>(this));
    }

    MessageHandler::~MessageHandler()
    {
        RemoveWindowSubclass(_window, HandlerProc, _id);
    }
}
