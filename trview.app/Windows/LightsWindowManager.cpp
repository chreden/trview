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
        auto lights_window = _lights_window_source();
        lights_window->set_level_version(_level_version);
        lights_window->set_lights(_lights);
        lights_window->set_selected_light(_selected_light);
        lights_window->set_current_room(_current_room);
        lights_window->on_light_selected += on_light_selected;
        lights_window->on_light_visibility += on_light_visibility;
        return add_window(lights_window);
    }

    void LightsWindowManager::render()
    {
        WindowManager::render();
    }
    
    void LightsWindowManager::update(float delta)
    {
        WindowManager::update(delta);
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
}
