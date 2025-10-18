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

    std::weak_ptr<ITriggersWindow> TriggersWindowManager::create_window()
    {
        auto triggers_window = _triggers_window_source();
        triggers_window->on_add_to_route += on_add_to_route;
        triggers_window->set_items(_items);
        triggers_window->set_platform_and_version(_platform_and_version);
        triggers_window->set_triggers(_triggers);
        triggers_window->set_current_room(_current_room);
        return add_window(triggers_window);
    }

    const std::weak_ptr<ITrigger> TriggersWindowManager::selected_trigger() const
    {
        return _selected_trigger;
    }

    void TriggersWindowManager::set_items(const std::vector<std::weak_ptr<IItem>>& items)
    {
        _items = items;
        for (auto& window : _windows)
        {
            window.second->set_items(items);
        }
    }

    void TriggersWindowManager::set_platform_and_version(const trlevel::PlatformAndVersion& platform_and_version)
    {
        _platform_and_version = platform_and_version;
        for (auto& window : _windows)
        {
            window.second->set_platform_and_version(platform_and_version);
        }
    }

    void TriggersWindowManager::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _triggers = triggers;
        _selected_trigger.reset();
        for (auto& window : _windows)
        {
            window.second->clear_selected_trigger();
            window.second->set_triggers(triggers);
        }
    }

    void TriggersWindowManager::set_room(const std::weak_ptr<IRoom>& room)
    {
        _current_room = room;
        for (auto& window : _windows)
        {
            window.second->set_current_room(room);
        }
    }

    void TriggersWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }
}
