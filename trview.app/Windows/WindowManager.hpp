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
                auto& window_list = _windows[window_number.first];
                window_list.erase(window_number.second);
            }
            _closing_windows.clear();
        }

        for (const auto& window_list : _windows)
        {
            for (auto& window : window_list.second)
            {
                window.second->render();
            }
        }
    }

    template <typename T>
    void WindowManager<T>::update(float delta)
    {
        for (const auto& window_list : _windows)
        {
            for (auto& window : window_list.second)
            {
                window.second->update(delta);
            }
        }
    }

    template <typename T>
    std::weak_ptr<T> WindowManager<T>::add_window(const std::shared_ptr<T>& window)
    {
        std::string type = window->type();
        int32_t number = next_id(type);
        window->set_number(number);
        _token_store += window->on_window_closed += [number, this, type]()
        {
            _closing_windows.push_back({ type, number });
        };
        _windows[type][number] = window;
        return window;
    }

    template <typename T>
    int32_t WindowManager<T>::next_id(const std::string& type) const
    {
        auto window_list = _windows.find(type);
        if (window_list != _windows.end())
        {
            for (int32_t i = 1;; ++i)
            {
                if (window_list->second.find(i) == window_list->second.end())
                {
                    return i;
                }
            }
        }
        return 1;
    }
}
