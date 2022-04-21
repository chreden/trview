#pragma once

namespace trview
{
    template <typename T>
    void WindowManager<T>::render()
    {
        if (!_closing_windows.empty())
        {
            for (const auto window_number : _closing_windows)
            {
                _windows.erase(window_number);
            }
            _closing_windows.clear();
        }

        for (auto& window : _windows)
        {
            window.second->render();
        }
    }

    template <typename T>
    void WindowManager<T>::update(float delta)
    {
        for (auto& window : _windows)
        {
            window.second->update(delta);
        }
    }

    template <typename T>
    std::weak_ptr<T> WindowManager<T>::add_window(const std::shared_ptr<T>& window)
    {
        int32_t number = next_id();
        window->set_number(number);
        _token_store += window->on_window_closed += [number, this]()
        {
            _closing_windows.push_back(number);
        };
        _windows[number] = window;
        return window;
    }

    template <typename T>
    int32_t WindowManager<T>::next_id() const
    {
        for (int32_t i = 1;; ++i)
        {
            if (_windows.find(i) == _windows.end())
            {
                return i;
            }
        }
    }
}
