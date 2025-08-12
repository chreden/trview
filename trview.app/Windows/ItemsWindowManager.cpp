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
        items_window->on_item_selected += on_item_selected;
        items_window->on_trigger_selected += on_trigger_selected;
        items_window->on_add_to_route += on_add_to_route;
        items_window->on_settings += on_settings;
        items_window->set_items(_items);
        items_window->set_triggers(_triggers);
        items_window->set_current_room(_current_room);
        items_window->set_level_version(_level_version);
        items_window->set_model_checker(_model_checker);
        items_window->set_ng_plus(_ng_plus);
        items_window->set_selected_item(_selected_item);
        items_window->set_settings(_settings);
        return add_window(items_window);
    }

    void ItemsWindowManager::set_items(const std::vector<std::weak_ptr<IItem>>& items)
    {
        _items = items;
        _selected_item.reset();
        for (auto& window : _windows)
        {
            window.second->clear_selected_item();
            window.second->set_items(items);
        }
    }

    void ItemsWindowManager::set_level_version(trlevel::LevelVersion version)
    {
        _level_version = version;
        for (auto& window : _windows)
        {
            window.second->set_level_version(version);
        }
    }

    void ItemsWindowManager::set_model_checker(const std::function<bool(uint32_t)>& checker)
    {
        _model_checker = checker;
        for (auto& window : _windows)
        {
            window.second->set_model_checker(checker);
        }
    }

    void ItemsWindowManager::set_ng_plus(bool value)
    {
        _ng_plus = value;
        for (auto& [_, window] : _windows)
        {
            window->set_ng_plus(value);
        }
    }

    void ItemsWindowManager::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _triggers = triggers;
        for (auto& window : _windows)
        {
            window.second->set_triggers(triggers);
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

    void ItemsWindowManager::set_selected_item(const std::weak_ptr<IItem>& item)
    {
        _selected_item = item;
        for (auto& window : _windows)
        {
            window.second->set_selected_item(item);
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

    void ItemsWindowManager::set_settings(const UserSettings& settings)
    {
        _settings = settings;
        for (auto& window : _windows)
        {
            window.second->set_settings(settings);
        }
    }
}
