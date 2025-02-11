#include "ItemsWindowManager.h"
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

    IItemsWindowManager::~IItemsWindowManager()
    {
    }

    ItemsWindowManager::ItemsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IItemsWindow::Source& items_window_source)
        : _items_window_source(items_window_source), MessageHandler(window)
    {
        _token_store += shortcuts->add_shortcut(true, 'I') += [&]() { create_window(); };
    }

    void ItemsWindowManager::add_level(const std::weak_ptr<ILevel>& level)
    {
        _levels.push_back({ .level = level });
        for (auto& window : _windows)
        {
            window.second->add_level(level);
        }
    }

    std::optional<int> ItemsWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_ITEMS)
        {
            create_window();
        }
        return {};
    }

    void ItemsWindowManager::render()
    {
        WindowManager::render();
    }

    std::weak_ptr<IItemsWindow> ItemsWindowManager::create_window()
    {
        auto items_window = _items_window_source();
        items_window->on_item_selected += on_item_selected;
        items_window->on_scene_changed += on_scene_changed;
        items_window->on_trigger_selected += on_trigger_selected;
        items_window->on_add_to_route += on_add_to_route;
        for (auto& level : _levels)
        {
            items_window->add_level(level.level);
            items_window->set_current_room(level.room);
            items_window->set_selected_item(level.item);
        }
        return add_window(items_window);
    }

    void ItemsWindowManager::set_room(const std::weak_ptr<IRoom>& room)
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

    void ItemsWindowManager::set_selected_item(const std::weak_ptr<IItem>& item)
    {
        if (const auto item_ptr = item.lock())
        {
            const auto found = find_level(_levels, item_ptr->level());
            if (found != _levels.end())
            {
                found->item = item;
                for (auto& window : _windows)
                {
                    window.second->set_selected_item(item);
                }
            }
        }
    }

    void ItemsWindowManager::update(float delta)
    {
        WindowManager::update(delta);
    }

    std::vector<std::weak_ptr<IItemsWindow>> ItemsWindowManager::windows() const
    {
        return _windows |
            std::views::transform([](auto&& w) -> std::weak_ptr<IItemsWindow> { return w.second; }) |
            std::ranges::to<std::vector>();
    }
}
