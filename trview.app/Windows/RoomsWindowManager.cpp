#include "RoomsWindowManager.h"
#include "../Resources/resource.h"
#include <trview.common/Windows/Shortcuts.h>

namespace trview
{
    IRoomsWindowManager::~IRoomsWindowManager()
    {
    }

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

    void RoomsWindowManager::set_items(const std::vector<std::weak_ptr<IItem>>& items)
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

    void RoomsWindowManager::set_selected_item(const std::weak_ptr<IItem>& item)
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

    std::weak_ptr<IRoomsWindow> RoomsWindowManager::create_window()
    {
        auto rooms_window = _rooms_window_source();
        rooms_window->on_room_selected += on_room_selected;
        rooms_window->on_item_selected += on_item_selected;
        rooms_window->on_trigger_selected += on_trigger_selected;
        rooms_window->on_room_visibility += on_room_visibility;
        rooms_window->on_sector_hover += on_sector_hover;
        rooms_window->on_light_selected += on_light_selected;
        rooms_window->on_camera_sink_selected += on_camera_sink_selected;
        rooms_window->on_static_mesh_selected += on_static_mesh_selected;
        rooms_window->set_level_version(_level_version);
        rooms_window->set_items(_all_items);
        rooms_window->set_rooms(_all_rooms);
        rooms_window->set_current_room(_current_room);
        rooms_window->set_map_colours(_map_colours);
        rooms_window->set_floordata(_floordata);
        rooms_window->set_selected_item(_selected_item);
        rooms_window->set_selected_trigger(_selected_trigger);
        rooms_window->set_selected_camera_sink(_selected_camera_sink);
        rooms_window->set_selected_light(_selected_light);
        return add_window(rooms_window);
    }

    void RoomsWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }

    void RoomsWindowManager::set_floordata(const std::vector<uint16_t>& data)
    {
        _floordata = data;
        for (auto& window : _windows)
        {
            window.second->set_floordata(data);
        }
    }

    void RoomsWindowManager::set_selected_light(const std::weak_ptr<ILight>& light)
    {
        _selected_light = light;
        for (auto& window : _windows)
        {
            window.second->set_selected_light(light);
        }
    }

    void RoomsWindowManager::set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink)
    {
        _selected_camera_sink = camera_sink;
        for (auto& window : _windows)
        {
            window.second->set_selected_camera_sink(camera_sink);
        }
    }
}