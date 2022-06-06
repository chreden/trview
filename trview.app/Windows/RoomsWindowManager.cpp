#include "RoomsWindowManager.h"
#include "../Resources/resource.h"
#include <trview.common/Windows/Shortcuts.h>

namespace trview
{
    RoomsWindowManager::RoomsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IRoomsWindow::Source& rooms_window_source)
        : MessageHandler(window), _rooms_window_source(rooms_window_source)
    {
        _token_store += shortcuts->add_shortcut(true, 'M') += [&]() { create_window(); };
    }

    std::optional<int> RoomsWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_ROOMS)
        {
            create_window();
        }
        return {};
    }

    void RoomsWindowManager::render()
    {
        WindowManager::render();
    }

    std::weak_ptr<ITrigger> RoomsWindowManager::selected_trigger() const
    {
        return _selected_trigger;
    }

    void RoomsWindowManager::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
        for (auto& window : _windows)
        {
            window.second->set_items(_all_items);
        }
    }

    void RoomsWindowManager::set_level_version(trlevel::LevelVersion version)
    {
        _level_version = version;
        for (auto& window : _windows)
        {
            window.second->set_level_version(_level_version);
        }
    }

    void RoomsWindowManager::set_map_colours(const MapColours& colours)
    {
        _map_colours = colours;
        for (auto& window : _windows)
        {
            window.second->set_map_colours(colours);
        }
    }

    void RoomsWindowManager::set_room(uint32_t room)
    {
        _current_room = room;
        for (auto& window : _windows)
        {
            window.second->set_current_room(room);
        }
    }

    void RoomsWindowManager::set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms)
    {
        _all_rooms = rooms;
        for (auto& window : _windows)
        {
            window.second->set_rooms(_all_rooms);
        }
    }

    void RoomsWindowManager::set_selected_item(const Item& item)
    {
        _selected_item = item;
        for (auto& window : _windows)
        {
            window.second->set_selected_item(item);
        }
    }

    void RoomsWindowManager::set_selected_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        _selected_trigger = trigger;
        for (auto& window : _windows)
        {
            window.second->set_selected_trigger(trigger);
        }
    }

    void RoomsWindowManager::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _all_triggers = triggers;
        _selected_trigger.reset();
        for (auto& window : _windows)
        {
            window.second->clear_selected_trigger();
            window.second->set_triggers(_all_triggers);
        }
    }

    std::weak_ptr<IRoomsWindow> RoomsWindowManager::create_window()
    {
        auto rooms_window = _rooms_window_source();
        rooms_window->on_room_selected += on_room_selected;
        rooms_window->on_item_selected += on_item_selected;
        rooms_window->on_trigger_selected += on_trigger_selected;
        rooms_window->on_room_visibility += on_room_visibility;
        rooms_window->set_level_version(_level_version);
        rooms_window->set_items(_all_items);
        rooms_window->set_triggers(_all_triggers);
        rooms_window->set_rooms(_all_rooms);
        rooms_window->set_current_room(_current_room);
        rooms_window->set_map_colours(_map_colours);
        return add_window(rooms_window);
    }

    void RoomsWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }
}