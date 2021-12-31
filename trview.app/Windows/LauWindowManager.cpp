#include "LauWindowManager.h"
#include <trview.common/Windows/IShortcuts.h>
#include "../Resources/resource.h"

namespace trview
{
    ILauWindowManager::~ILauWindowManager()
    {
    }

    LauWindowManager::LauWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const ILauWindow::Source& lau_window_source)
        : MessageHandler(window), _lau_window_source(lau_window_source)
    {
        _token_store += shortcuts->add_shortcut(true, 'U') += [&]() { create_window(); };
    }

    std::optional<int> LauWindowManager::process_message(UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_LAU)
        {
            create_window();
        }
        return {};
    }

    void LauWindowManager::render(bool vsync)
    {
        if (!_closing_windows.empty())
        {
            for (const auto window_ptr : _closing_windows)
            {
                auto window = window_ptr.lock();
                _windows.erase(std::remove(_windows.begin(), _windows.end(), window));
            }
            _closing_windows.clear();
        }

        for (auto& window : _windows)
        {
            window->render(vsync);
        }
    }

    std::weak_ptr<ILauWindow> LauWindowManager::create_window()
    {
        auto lau_window = _lau_window_source(window());

        _token_store += lau_window->on_window_closed += [lau_window, this]()
        {
            _closing_windows.push_back(lau_window);
        };
        lau_window->on_mesh += on_mesh;

        _windows.push_back(lau_window);
        return lau_window;
    }
}
