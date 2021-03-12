#include "ItemsWindowManager.h"
#include <trview.app/Windows/WindowIDs.h>
#include <trview.ui.render/Renderer.h>
#include <trview.graphics/DeviceWindow.h>

namespace trview
{
    ItemsWindowManager::ItemsWindowManager(graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::IFontFactory& font_factory, const Window& window, IShortcuts& shortcuts)
        : _device(device), _shader_storage(shader_storage), _font_factory(font_factory), MessageHandler(window)
    {
        _token_store += shortcuts.add_shortcut(true, 'I') += [&]() { create_window(); };
    }

    void ItemsWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_APP_WINDOWS_ITEMS)
        {
            create_window();
        }
    }

    void ItemsWindowManager::render(graphics::Device& device, bool vsync)
    {
        if (!_closing_windows.empty())
        {
            _windows.erase(std::remove_if(_windows.begin(), _windows.end(),
                [&](auto& window) { return std::find(_closing_windows.begin(), _closing_windows.end(), window.get()) != _closing_windows.end(); }), _windows.end());
            _closing_windows.clear();
        }

        for (auto& window : _windows)
        {
            window->render(device, vsync);
        }
    }

    ItemsWindow* ItemsWindowManager::create_window()
    {
        auto items_window = std::make_unique<ItemsWindow>(_device, _shader_storage, _font_factory, window());
        items_window->on_item_selected += on_item_selected;
        items_window->on_item_visibility += on_item_visibility;
        items_window->on_trigger_selected += on_trigger_selected;
        items_window->on_add_to_route += on_add_to_route;
        items_window->set_items(_items);
        items_window->set_triggers(_triggers);
        items_window->set_current_room(_current_room);
        if (_selected_item.has_value())
        {
            items_window->set_selected_item(_selected_item.value());
        }

        const auto window = items_window.get();
        _token_store += items_window->on_window_closed += [window, this]()
        {
            _closing_windows.push_back(window);
        };

        _windows.push_back(std::move(items_window));
        return window;
    }

    void ItemsWindowManager::set_items(const std::vector<Item>& items)
    {
        _items = items;
        for (auto& window : _windows)
        {
            window->clear_selected_item();
            window->set_items(items);
        }
    }

    void ItemsWindowManager::set_item_visible(const Item& item, bool visible)
    {
        auto found = std::find_if(_items.begin(), _items.end(), [&item](const auto& l) { return l.number() == item.number(); });
        if (found == _items.end())
        {
            return;
        }
        found->set_visible(visible);
        for (auto& window : _windows)
        {
            window->update_items(_items);
        }
    }

    void ItemsWindowManager::set_triggers(const std::vector<Trigger*>& triggers)
    {
        _triggers = triggers;
        for (auto& window : _windows)
        {
            window->set_triggers(triggers);
        }
    }

    void ItemsWindowManager::set_room(uint32_t room)
    {
        _current_room = room;
        for (auto& window : _windows)
        {
            window->set_current_room(room);
        }
    }

    void ItemsWindowManager::set_selected_item(const Item& item)
    {
        _selected_item = item;
        for (auto& window : _windows)
        {
            window->set_selected_item(item);
        }
    }
}
