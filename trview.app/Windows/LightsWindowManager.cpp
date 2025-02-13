#include "LightsWindowManager.h"
#include "../Resources/resource.h"
#include "../Elements/ILight.h"
#include "../Elements/IRoom.h"

namespace trview
{
    namespace
    {
        auto find_level(auto&& levels, auto&& level)
        {
            return std::ranges::find_if(levels, [=](auto&& l) { return l.level.lock() == level.lock(); });
        }
    }

    ILightsWindowManager::~ILightsWindowManager()
    {
    }

    LightsWindowManager::LightsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const ILightsWindow::Source& lights_window_source)
        : _lights_window_source(lights_window_source), MessageHandler(window)
    {
        _token_store += shortcuts->add_shortcut(true, 'L') += [&]() { create_window(); };
    }

    void LightsWindowManager::add_level(const std::weak_ptr<ILevel>& level)
    {
        _levels.push_back({ .level = level });
        for (auto& window : _windows)
        {
            window.second->add_level(level);
        }
    }

    std::optional<int> LightsWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_LIGHTS)
        {
            create_window();
        }
        return {};
    }

    std::weak_ptr<ILightsWindow> LightsWindowManager::create_window()
    {
        auto lights_window = _lights_window_source();
        lights_window->on_light_selected += on_light_selected;
        lights_window->on_scene_changed += on_scene_changed;
        for (auto& level : _levels)
        {
            lights_window->add_level(level.level);
            lights_window->set_current_room(level.room);
            lights_window->set_selected_light(level.light);
        }
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
        if (const auto light_ptr = light.lock())
        {
            const auto found = find_level(_levels, light_ptr->level());
            if (found != _levels.end())
            {
                found->light = light;
                for (auto& window : _windows)
                {
                    window.second->set_selected_light(light);
                }
            }
        }
    }

    void LightsWindowManager::set_room(const std::weak_ptr<IRoom>& room)
    {
        if (const auto room_ptr = room.lock())
        {
            const auto found = find_level(_levels, room_ptr->level());
            if (found != _levels.end())
            {
                found->room = room;
                for (auto& window : _windows)
                {
                    window.second->set_current_room(room);
                }
            }
        }
    }
}
