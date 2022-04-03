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
            window->clear_selected_light();
            window->set_lights(lights);
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
            window->update_lights(_lights);
        }
    }

    void LightsWindowManager::set_level_version(trlevel::LevelVersion version)
    {
        _level_version = version;
        for (auto& window : _windows)
        {
            window->set_level_version(_level_version);
        }
    }

    std::weak_ptr<ILightsWindow> LightsWindowManager::create_window()
    {
        auto lights_window = _lights_window_source();
        lights_window->set_number(++_window_count);
        lights_window->set_level_version(_level_version);
        lights_window->set_lights(_lights);
        lights_window->set_selected_light(_selected_light);
        lights_window->set_current_room(_current_room);

        std::weak_ptr<ILightsWindow> lights_window_weak = lights_window;
        _token_store += lights_window->on_window_closed += [lights_window_weak, this]()
        {
            _closing_windows.push_back(lights_window_weak);
        };
        lights_window->on_light_selected += on_light_selected;
        lights_window->on_light_visibility += on_light_visibility;

        _windows.push_back(lights_window);
        return lights_window;
    }

    void LightsWindowManager::render()
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
            window->render();
        }
    }
    
    void LightsWindowManager::update(float delta)
    {
        for (auto& window : _windows)
        {
            window->update(delta);
        }
    }

    void LightsWindowManager::set_selected_light(const std::weak_ptr<ILight>& light)
    {
        _selected_light = light;
        for (auto& window : _windows)
        {
            window->set_selected_light(light);
        }
    }

    void LightsWindowManager::set_room(uint32_t room)
    {
        _current_room = room;
        for (auto& window : _windows)
        {
            window->set_current_room(room);
        }
    }
}
