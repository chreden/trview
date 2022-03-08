#include "FloordataWindowManager.h"
#include "../../Resources/resource.h"

namespace trview
{
    IFloordataWindowManager::~IFloordataWindowManager()
    {
    }

    FloordataWindowManager::FloordataWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IFloordataWindow::Source& floordata_window_source)
        : MessageHandler(window), _floordata_window_source(floordata_window_source)
    {
        _token_store += shortcuts->add_shortcut(true, 'D') += [&]() { create_window(); };
    }

    std::weak_ptr<IFloordataWindow> FloordataWindowManager::create_window()
    {
        auto floordata_window = _floordata_window_source();
        floordata_window->set_rooms(_rooms);
        floordata_window->set_items(_items);
        floordata_window->set_floordata(_floordata);
        floordata_window->set_current_room(_current_room);
        floordata_window->on_sector_highlight += on_sector_highlight;
        return add_window(floordata_window);
    }

    std::optional<int> FloordataWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_FLOORDATA)
        {
            create_window();
        }
        return {};
    }

    void FloordataWindowManager::render()
    {
        WindowManager::render();
    }

    void FloordataWindowManager::set_room(uint32_t room)
    {
        _current_room = room;
        for (auto& window : _windows)
        {
            window.second->set_current_room(_current_room);
        }
    }

    void FloordataWindowManager::set_floordata(const std::vector<uint16_t>& data)
    {
        _floordata = data;
        for (auto& window : _windows)
        {
            window.second->set_floordata(_floordata);
        }
    }

    void FloordataWindowManager::set_items(const std::vector<Item>& items)
    {
        _items = items;
        for (auto& window : _windows)
        {
            window.second->set_items(items);
        }
    }

    void FloordataWindowManager::set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms)
    {
        _rooms = rooms;
        for (auto& window : _windows)
        {
            window.second->set_rooms(_rooms);
        }
    }

    void FloordataWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }
}
