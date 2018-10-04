#include "TriggersWindowManager.h"
#include "WindowIDs.h"
#include <trview.ui.render/Renderer.h>
#include <trview.graphics/DeviceWindow.h>

namespace trview
{
    TriggersWindowManager::TriggersWindowManager(graphics::Device& device, graphics::IShaderStorage& shader_storage, graphics::FontFactory& font_factory, HWND window)
        : _device(device), _shader_storage(shader_storage), _font_factory(font_factory), MessageHandler(window)
    {
    }

    void TriggersWindowManager::process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_COMMAND &&
            LOWORD(wParam) == ID_APP_WINDOWS_ITEMS ||
            LOWORD(wParam) == ID_APP_ACCEL_ITEM_WINDOW)
        {
            create_window();
        }
    }

    void TriggersWindowManager::render(graphics::Device& device, bool vsync)
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

    void TriggersWindowManager::create_window()
    {
        auto items_window = std::make_unique<TriggersWindow>(_device, _shader_storage, _font_factory, window());
        items_window->on_item_selected += on_item_selected;
        items_window->on_trigger_selected += on_trigger_selected;
        // items_window->set_items(_items);
        items_window->set_triggers(_triggers);
        items_window->set_current_room(_current_room);

        const auto window = items_window.get();
        _token_store.add(items_window->on_window_closed += [window, this]()
        {
            _closing_windows.push_back(window);
        });

        _windows.push_back(std::move(items_window));
    }

    void TriggersWindowManager::set_items(const std::vector<Item>& items)
    {
        _items = items;
        for (auto& window : _windows)
        {
            window->set_items(items);
        }
    }

    void TriggersWindowManager::set_triggers(const std::vector<Trigger>& triggers)
    {
        _triggers = triggers;
        for (auto& window : _windows)
        {
            window->clear_selected_trigger();
            window->set_triggers(triggers);
        }
    }

    void TriggersWindowManager::set_room(uint32_t room)
    {
        _current_room = room;
        for (auto& window : _windows)
        {
            window->set_current_room(room);
        }
    }

    void TriggersWindowManager::set_selected_trigger(const Trigger& trigger)
    {
        for (auto& window : _windows)
        {
            window->set_selected_trigger(trigger);
        }
    }
}
