#include "TriggersWindowManager.h"
#include "../Resources/resource.h"

namespace trview
{
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
        _levels.push_back(level);
        for (auto& window : _windows)
        {
            window.second->add_level(level);
        }
    }

    std::weak_ptr<ITriggersWindow> TriggersWindowManager::create_window()
    {
        auto triggers_window = _triggers_window_source();
        for (auto& level : _levels)
        {
            triggers_window->add_level(level);
        }
        triggers_window->on_item_selected += on_item_selected;
        triggers_window->on_trigger_selected += on_trigger_selected;
        triggers_window->on_scene_changed += on_scene_changed;
        triggers_window->on_add_to_route += on_add_to_route;
        triggers_window->on_camera_sink_selected += on_camera_sink_selected;
        triggers_window->set_current_room(_current_room);
        triggers_window->set_selected_trigger(_selected_trigger);
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

    const std::weak_ptr<ITrigger> TriggersWindowManager::selected_trigger() const
    {
        return _selected_trigger;
    }

    void TriggersWindowManager::set_room(const std::weak_ptr<IRoom>& room)
    {
        _current_room = room;
        for (auto& window : _windows)
        {
            window.second->set_current_room(room);
        }
    }

    void TriggersWindowManager::set_selected_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        _selected_trigger = trigger;
        for (auto& window : _windows)
        {
            window.second->set_selected_trigger(trigger);
        }
    }

    void TriggersWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }
}
