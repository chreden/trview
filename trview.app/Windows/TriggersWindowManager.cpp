#include "TriggersWindowManager.h"
#include <trview.app/Windows/WindowIDs.h>
#include <trview.ui.render/Renderer.h>
#include <trview.graphics/DeviceWindow.h>
#include <trview.common/Windows/Shortcuts.h>

namespace trview
{
    TriggersWindowManager::TriggersWindowManager(graphics::Device& device, graphics::IShaderStorage& shader_storage, graphics::IFontFactory& font_factory, const Window& window, Shortcuts& shortcuts)
        : _device(device), _shader_storage(shader_storage), _font_factory(font_factory), MessageHandler(window)
    {
        _token_store += shortcuts.add_shortcut(true, 'T') += [&]() { create_window(); };
    }

    void TriggersWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_APP_WINDOWS_TRIGGERS)
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

    TriggersWindow* TriggersWindowManager::create_window()
    {
        auto triggers_window = std::make_unique<TriggersWindow>(_device, _shader_storage, _font_factory, window());
        triggers_window->on_item_selected += on_item_selected;
        triggers_window->on_trigger_selected += on_trigger_selected;
        triggers_window->on_trigger_visibility += on_trigger_visibility;
        triggers_window->on_add_to_route += on_add_to_route;
        triggers_window->set_items(_items);
        triggers_window->set_triggers(_triggers);
        triggers_window->set_current_room(_current_room);
        if (_selected_trigger.has_value())
        {
            triggers_window->set_selected_trigger(_selected_trigger.value());
        }

        const auto window = triggers_window.get();
        _token_store += triggers_window->on_window_closed += [window, this]()
        {
            _closing_windows.push_back(window);
        };

        _windows.push_back(std::move(triggers_window));
        return window;
    }

    void TriggersWindowManager::set_items(const std::vector<Item>& items)
    {
        _items = items;
        for (auto& window : _windows)
        {
            window->set_items(items);
        }
    }

    void TriggersWindowManager::set_triggers(const std::vector<Trigger*>& triggers)
    {
        _triggers = triggers;
        for (auto& window : _windows)
        {
            window->clear_selected_trigger();
            window->set_triggers(triggers);
        }
    }

    void TriggersWindowManager::set_trigger_visible(Trigger* trigger, bool visible)
    {
        auto found = std::find(_triggers.begin(), _triggers.end(), trigger);
        if (found == _triggers.end())
        {
            return;
        }
        trigger->set_visible(visible);
        for (auto& window : _windows)
        {
            window->update_triggers(_triggers);
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

    void TriggersWindowManager::set_selected_trigger(const Trigger* const trigger)
    {
        _selected_trigger = trigger;
        for (auto& window : _windows)
        {
            window->set_selected_trigger(trigger);
        }
    }
}
