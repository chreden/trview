#include "TriggersWindowManager.h"
#include "../Resources/resource.h"
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

    ITriggersWindowManager::~ITriggersWindowManager()
    {
    }

    TriggersWindowManager::TriggersWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const ITriggersWindow::Source& triggers_window_source)
        : _triggers_window_source(triggers_window_source), MessageHandler(window)
    {
        _token_store += shortcuts->add_shortcut(true, 'T') += [&]() { create_window(); };
    }

    void TriggersWindowManager::add_level(const std::weak_ptr<ILevel>& level)
    {
        _levels.push_back({ .level = level });
        for (auto& window : _windows)
        {
            window.second->add_level(level);
        }
    }

    std::weak_ptr<ITriggersWindow> TriggersWindowManager::create_window()
    {
        auto triggers_window = _triggers_window_source();
        triggers_window->on_item_selected += on_item_selected;
        triggers_window->on_trigger_selected += on_trigger_selected;
        triggers_window->on_scene_changed += on_scene_changed;
        triggers_window->on_add_to_route += on_add_to_route;
        triggers_window->on_camera_sink_selected += on_camera_sink_selected;
        for (auto& level : _levels)
        {
            triggers_window->add_level(level.level);
            triggers_window->set_current_room(level.room);
            triggers_window->set_selected_trigger(level.trigger);
        }
        return add_window(triggers_window);
    }

    std::optional<int> TriggersWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_TRIGGERS)
        {
            create_window();
        }
        return {};
    }

    void TriggersWindowManager::render()
    {
        WindowManager::render();
    }

    void TriggersWindowManager::set_room(const std::weak_ptr<IRoom>& room)
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

    void TriggersWindowManager::set_selected_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        if (const auto trigger_ptr = trigger.lock())
        {
            const auto found = find_level(_levels, trigger_ptr->level());
            if (found != _levels.end())
            {
                found->trigger = trigger;
                for (auto& window : _windows)
                {
                    window.second->set_selected_trigger(trigger);
                }
            }
        }
    }

    void TriggersWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }
}
