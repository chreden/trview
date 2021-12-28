#include "LauWindowManager.h"

namespace trview
{
    ILauWindowManager::~ILauWindowManager()
    {
    }

    LauWindowManager::LauWindowManager(const Window& window, const ILauWindow::Source& lau_window_source)
        : MessageHandler(window), _lau_window_source(lau_window_source)
    {
    }

    std::optional<int> LauWindowManager::process_message(UINT message, WPARAM wParam, LPARAM lParam)
    {
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

        _windows.push_back(lau_window);
        return lau_window;
    }
}
