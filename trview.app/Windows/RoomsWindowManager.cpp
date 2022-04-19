#include "RoomsWindowManager.h"
#include <trview.app/Windows/WindowIDs.h>
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
        if (message == WM_COMMAND && LOWORD(wParam) == ID_APP_WINDOWS_ROOMS)
        {
            create_window();
        }
        return {};
    }

    void RoomsWindowManager::render()
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
        int32_t number = next_id();
        auto rooms_window = _rooms_window_source();
        rooms_window->set_number(number);
        rooms_window->on_room_selected += on_room_selected;
        rooms_window->on_item_selected += on_item_selected;
        rooms_window->on_trigger_selected += on_trigger_selected;

        _token_store += rooms_window->on_window_closed += [number, this]()
        {
            _closing_windows.push_back(number);
        };

        rooms_window->set_level_version(_level_version);
        rooms_window->set_items(_all_items);
        rooms_window->set_triggers(_all_triggers);
        rooms_window->set_rooms(_all_rooms);
        rooms_window->set_current_room(_current_room);
        rooms_window->set_map_colours(_map_colours);

        _windows[number] = rooms_window;
        return rooms_window;
    }

    void RoomsWindowManager::update(float delta)
    {
        for (auto& window : _windows)
        {
            window.second->update(delta);
        }
    }

    int32_t RoomsWindowManager::next_id() const
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