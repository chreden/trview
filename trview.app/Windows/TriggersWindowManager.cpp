#include "TriggersWindowManager.h"
#include <trview.app/Windows/WindowIDs.h>
#include <trview.common/Windows/Shortcuts.h>

namespace trview
{
    TriggersWindowManager::TriggersWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const ITriggersWindow::Source& triggers_window_source)
        : _triggers_window_source(triggers_window_source), MessageHandler(window)
    {
        _token_store += shortcuts->add_shortcut(true, 'T') += [&]() { create_window(); };
    }

    std::optional<int> TriggersWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_APP_WINDOWS_TRIGGERS)
        {
            create_window();
        }
        return {};
    }

    void TriggersWindowManager::render(bool vsync)
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

    std::weak_ptr<ITriggersWindow> TriggersWindowManager::create_window()
    {
        auto triggers_window = _triggers_window_source(window());
        triggers_window->on_item_selected += on_item_selected;
        triggers_window->on_trigger_selected += on_trigger_selected;
        triggers_window->on_trigger_visibility += on_trigger_visibility;
        triggers_window->on_add_to_route += on_add_to_route;
        triggers_window->set_items(_items);
        triggers_window->set_triggers(_triggers);
        triggers_window->set_current_room(_current_room);
        triggers_window->set_selected_trigger(_selected_trigger);

        _token_store += triggers_window->on_window_closed += [triggers_window, this]()
        {
            _closing_windows.push_back(triggers_window);
        };

        _windows.push_back(triggers_window);
        return triggers_window;
    }

    const std::weak_ptr<ITrigger> TriggersWindowManager::selected_trigger() const
    {
        return _selected_trigger;
    }

    void TriggersWindowManager::set_items(const std::vector<Item>& items)
    {
        _items = items;
        for (auto& window : _windows)
        {
            window->set_items(items);
        }
    }

    void TriggersWindowManager::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _triggers = triggers;
        _selected_trigger.reset();
        for (auto& window : _windows)
        {
            window->clear_selected_trigger();
            window->set_triggers(triggers);
        }
    }

    void TriggersWindowManager::set_trigger_visible(const std::weak_ptr<ITrigger>& trigger, bool visible)
    {
        const auto trigger_ptr = trigger.lock();
        auto found = std::find_if(_triggers.begin(), _triggers.end(), 
            [&](const auto& t) 
            {
                return t.lock() == trigger_ptr;
            });
        if (found == _triggers.end())
        {
            return;
        }
        
        trigger_ptr->set_visible(visible);
        for (auto& window : _windows)
        {
            window->update_triggers(_triggers);
        }
    }

    void TriggersWindowManager::set_room(uint32_t room)
    {
        _current_room = room;
        for (auto& window : _windows)
        {
            window->set_current_room(room);
        }
    }

    void TriggersWindowManager::set_selected_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        _selected_trigger = trigger;
        for (auto& window : _windows)
        {
            window->set_selected_trigger(trigger);
        }
    }
}
