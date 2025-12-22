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

    void RoomsWindowManager::set_items(const std::vector<std::weak_ptr<IItem>>& items)
    {
        _all_items = items;
        for (auto& window : _windows)
        {
            window.second->set_items(_all_items);
        }
    }

    void RoomsWindowManager::set_room(const std::weak_ptr<IRoom>& room)
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

    std::weak_ptr<IRoomsWindow> RoomsWindowManager::create_window()
    {
        auto rooms_window = _rooms_window_source();
        rooms_window->on_sector_hover += on_sector_hover;
        rooms_window->on_static_mesh_selected += on_static_mesh_selected;
        rooms_window->set_items(_all_items);
        rooms_window->set_rooms(_all_rooms);
        rooms_window->set_current_room(_current_room);
        rooms_window->set_floordata(_floordata);
        rooms_window->set_trng(_trng);
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

    void RoomsWindowManager::set_trng(bool value)
    {
        _trng = value;
        for (auto& [_, window] : _windows)
        {
            window->set_trng(value);
        }
    }

    std::vector<std::weak_ptr<IRoomsWindow>> RoomsWindowManager::windows() const
    {
        return _windows |
            std::views::transform([](auto&& w) -> std::weak_ptr<IRoomsWindow> { return w.second; }) |
            std::ranges::to<std::vector>();
    }
}