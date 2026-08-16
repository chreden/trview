#include "Windows.h"
#include "Settings/UserSettings.h"
#include "IWindow.h"
#include "../Messages/Messages.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    IWindow::~IWindow()
    {
    }

    IWindows::~IWindows()
    {
    }

    std::weak_ptr<IWindow> Windows::create(const std::string& type)
    {
        const auto creator = _creators.find(type);
        if (creator == _creators.end())
        {
            return {};
        }
        return add_window(creator->second());
    }

    std::weak_ptr<IWindow> Windows::add_window(const std::shared_ptr<IWindow>& window)
    {
        std::string type = window->type();
        const int32_t number = next_id(type);
        window->set_number(number);
        _token_store += window->on_window_closed += [number, this, type]()
            {
                _closing_windows.push_back({ type, number });
            };
        _windows[type][number] = window;
        return window;
    }

    void Windows::update(float elapsed)
    {
        for (const auto& window_list : _windows)
        {
            for (auto& window : window_list.second)
            {
                window.second->update(elapsed);
            }
        }
    }

    void Windows::register_window(const std::string& type, const Creator& creator)
    {
        _creators[type] = creator;
    }

    void Windows::render()
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

    void Windows::setup(const UserSettings& settings)
    {
        if (settings.camera_sink_startup)
        {
            create("CameraSink");
        }

        if (settings.items_startup)
        {
            create("Items");
        }

        if (settings.rooms_startup)
        {
            create("Rooms");
        }

        if (settings.route_startup)
        {
            create("Route");
        }

        if (settings.statics_startup)
        {
            create("Statics");
        }

        if (settings.triggers_startup)
        {
            create("Triggers");
        }
    }

    std::vector<std::weak_ptr<IWindow>> Windows::windows(const std::string& type) const
    {
        std::vector<std::weak_ptr<IWindow>> results;
        auto window_list = _windows.find(type);
        if (window_list != _windows.end())
        {
            for (auto& window : window_list->second)
            {
                results.push_back(window.second);
            }
        }
        return results;
    }

    int32_t Windows::next_id(const std::string& type) const
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

    void Windows::receive_message(const Message& message)
    {
        if (const auto window = messages::commands::read_create_window(message))
        {
            create(window.value());
        }
    }
}
