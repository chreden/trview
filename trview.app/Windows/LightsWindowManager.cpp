#include "LightsWindowManager.h"
#include <trview.app/Windows/WindowIDs.h>

namespace trview
{
    LightsWindowManager::LightsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const ILightsWindow::Source& lights_window_source)
        : _lights_window_source(lights_window_source), MessageHandler(window)
    {
        _token_store += shortcuts->add_shortcut(true, 'L') += [&]() { create_window(); };
    }

    std::optional<int> LightsWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_APP_WINDOWS_LIGHTS)
        {
            create_window();
        }
        return {};
    }

    void LightsWindowManager::set_lights(const std::vector<std::weak_ptr<ILight>>& lights)
    {
        _lights = lights;
        _selected_light.reset();
        for (auto& window : _windows)
        {
            window.second->clear_selected_light();
            window.second->set_lights(lights);
        }
    }

    void LightsWindowManager::set_light_visible(const std::weak_ptr<ILight>& light, bool state)
    {
        const auto light_ptr = light.lock();
        auto found = std::find_if(_lights.begin(), _lights.end(),
            [&](const auto& l)
            {
                return l.lock() == light_ptr;
            });
        if (found == _lights.end())
        {
            return;
        }

        light_ptr->set_visible(state);
        for (auto& window : _windows)
        {
            window.second->update_lights(_lights);
        }
    }

    void LightsWindowManager::set_level_version(trlevel::LevelVersion version)
    {
        _level_version = version;
        for (auto& window : _windows)
        {
            window.second->set_level_version(_level_version);
        }
    }

    std::weak_ptr<ILightsWindow> LightsWindowManager::create_window()
    {
        int32_t number = next_id();
        auto lights_window = _lights_window_source();
        lights_window->set_number(number);
        lights_window->set_level_version(_level_version);
        lights_window->set_lights(_lights);
        lights_window->set_selected_light(_selected_light);
        lights_window->set_current_room(_current_room);

        _token_store += lights_window->on_window_closed += [number, this]()
        {
            _closing_windows.push_back(number);
        };
        lights_window->on_light_selected += on_light_selected;
        lights_window->on_light_visibility += on_light_visibility;

        _windows[number] = lights_window;
        return lights_window;
    }

    void LightsWindowManager::render()
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
    
    void LightsWindowManager::update(float delta)
    {
        for (auto& window : _windows)
        {
            window.second->update(delta);
        }
    }

    void LightsWindowManager::set_selected_light(const std::weak_ptr<ILight>& light)
    {
        _selected_light = light;
        for (auto& window : _windows)
        {
            window.second->set_selected_light(light);
        }
    }

    void LightsWindowManager::set_room(uint32_t room)
    {
        _current_room = room;
        for (auto& window : _windows)
        {
            window.second->set_current_room(room);
        }
    }

    int32_t LightsWindowManager::next_id() const
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
