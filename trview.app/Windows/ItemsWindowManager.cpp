#include "ItemsWindowManager.h"
#include "../Resources/resource.h"

namespace trview
{
    IItemsWindowManager::~IItemsWindowManager()
    {
    }

    ItemsWindowManager::ItemsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IItemsWindow::Source& items_window_source)
        : _items_window_source(items_window_source), MessageHandler(window)
    {
        _token_store += shortcuts->add_shortcut(true, 'I') += [&]() { create_window(); };
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
        items_window->on_add_to_route += on_add_to_route;
        items_window->set_current_room(_current_room);
        items_window->set_ng_plus(_ng_plus);
        return add_window(items_window);
    }

    void ItemsWindowManager::set_ng_plus(bool value)
    {
        _ng_plus = value;
        for (auto& [_, window] : _windows)
        {
            window->set_ng_plus(value);
        }
    }

    void ItemsWindowManager::set_room(const std::weak_ptr<IRoom>& room)
    {
        _current_room = room;
        for (auto& window : _windows)
        {
            window.second->set_current_room(room);
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
