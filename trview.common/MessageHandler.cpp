#include "stdafx.h"
#include "MessageHandler.h"
#include <CommCtrl.h>
#include <random>

namespace trview
{
    namespace
    {
        // Window procedure - will forward messages to the handler in the reference data.
        LRESULT CALLBACK HandlerProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR dwRefData)
        {
            MessageHandler* handler = reinterpret_cast<MessageHandler*>(dwRefData);
            if (handler)
            {
                handler->process_message(hWnd, message, wParam, lParam);
            }
            return DefSubclassProc(hWnd, message, wParam, lParam);
        }

        /// Find the next available subclass id.
        /// @param window The window to subclass.
        /// @remarks This will test random subclass IDs until it finds one that isn't set.
        uint32_t next_subclass_id(HWND window)
        {
            std::default_random_engine random;
            uint32_t id = random();
            DWORD_PTR existing = 0;
            while (GetWindowSubclass(window, HandlerProc, id, &existing))
            {
                id = random();
            }
            return id;
        }
    }

    MessageHandler::MessageHandler(HWND window)
        : _window(window), _subclass_id(next_subclass_id(window))
    {
        SetWindowSubclass(window, HandlerProc, _subclass_id, reinterpret_cast<DWORD_PTR>(this));
    }

    MessageHandler::MessageHandler(const MessageHandler& other)
        : MessageHandler(other._window)
    {
    }

    MessageHandler::MessageHandler(MessageHandler&& other)
        : _window(other._window), _subclass_id(other._subclass_id)
    {
        other._window = 0;
        other._subclass_id = 0;
        SetWindowSubclass(_window, HandlerProc, _subclass_id, reinterpret_cast<DWORD_PTR>(this));
    }

    MessageHandler& MessageHandler::operator=(const MessageHandler& other)
    {
        _window = other._window;
        _subclass_id = next_subclass_id(_window);
        SetWindowSubclass(_window, HandlerProc, _subclass_id, reinterpret_cast<DWORD_PTR>(this));
    }

    MessageHandler& MessageHandler::operator=(MessageHandler&& other)
    {
        _window = other._window;
        _subclass_id = other._subclass_id;
        SetWindowSubclass(_window, HandlerProc, _subclass_id, reinterpret_cast<DWORD_PTR>(this));
    }

    MessageHandler::~MessageHandler()
    {
        RemoveWindowSubclass(_window, HandlerProc, _subclass_id);
    }

    HWND MessageHandler::window() const
    {
        return _window;
    }
}
