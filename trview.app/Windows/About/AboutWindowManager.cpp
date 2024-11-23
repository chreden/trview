#include "AboutWindowManager.h"
#include "IAboutWindow.h"
#include "../../Resources/resource.h"

namespace trview
{
    IAboutWindowManager::~IAboutWindowManager()
    {
    }

    AboutWindowManager::AboutWindowManager(const Window& window, const IAboutWindow::Source& about_window_source)
        : MessageHandler(window), _about_window_source(about_window_source)
    {
    }

    void AboutWindowManager::create_window()
    {
        // If the window already exists, just focus on the window.
        if (_about_window)
        {
            _about_window->focus();
            return;
        }

        _about_window = _about_window_source();
        _token_store += _about_window->on_window_closed += [&]() { _closing = true; };
    }

    std::optional<int> AboutWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == IDM_ABOUT)
        {
            create_window();
        }
        return {};
    }

    void AboutWindowManager::render()
    {
        if (_closing)
        {
            _about_window.reset();
            _closing = false;
        }

        if (_about_window)
        {
            _about_window->render();
        }
    }
}
