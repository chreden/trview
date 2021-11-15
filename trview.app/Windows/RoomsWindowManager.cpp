#include "RoomsWindowManager.h"
#include <trview.app/Windows/WindowIDs.h>
#include <trview.ui.render/Renderer.h>
#include <trview.graphics/DeviceWindow.h>
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

    void RoomsWindowManager::render(bool vsync)
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

    std::weak_ptr<ITrigger> RoomsWindowManager::selected_trigger() const
    {
        return _selected_trigger;
    }

    void RoomsWindowManager::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
        for (auto& window : _windows)
        {
            window->set_items(_all_items);
        }
    }

    void RoomsWindowManager::set_level_version(trlevel::LevelVersion version)
    {
        _level_version = version;
        for (auto& window : _windows)
        {
            window->set_level_version(_level_version);
        }
    }

    void RoomsWindowManager::set_room(uint32_t room)
    {
        _current_room = room;
        for (auto& window : _windows)
        {
            window->set_current_room(room);
        }
    }

    void RoomsWindowManager::set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms)
    {
        _all_rooms = rooms;
        for (auto& window : _windows)
        {
            window->set_rooms(_all_rooms);
        }
    }

    void RoomsWindowManager::set_selected_item(const Item& item)
    {
        _selected_item = item;
        for (auto& window : _windows)
        {
            window->set_selected_item(item);
        }
    }

    void RoomsWindowManager::set_selected_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        _selected_trigger = trigger;
        for (auto& window : _windows)
        {
            window->set_selected_trigger(trigger);
        }
    }

    void RoomsWindowManager::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _all_triggers = triggers;
        _selected_trigger.reset();
        for (auto& window : _windows)
        {
            window->clear_selected_trigger();
            window->set_triggers(_all_triggers);
        }
    }

    std::weak_ptr<IRoomsWindow> RoomsWindowManager::create_window()
    {
        auto rooms_window = _rooms_window_source(window());
        rooms_window->on_room_selected += on_room_selected;
        rooms_window->on_item_selected += on_item_selected;
        rooms_window->on_trigger_selected += on_trigger_selected;

        _token_store += rooms_window->on_window_closed += [rooms_window, this]()
        {
            _closing_windows.push_back(rooms_window);
        };

        rooms_window->set_level_version(_level_version);
        rooms_window->set_items(_all_items);
        rooms_window->set_triggers(_all_triggers);
        rooms_window->set_rooms(_all_rooms);
        rooms_window->set_current_room(_current_room);

        _windows.push_back(rooms_window);
        return rooms_window;
    }

    void RoomsWindowManager::update(float delta)
    {
        for (auto& window : _windows)
        {
            window->update(delta);
        }
    }
}