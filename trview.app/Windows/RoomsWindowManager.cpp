#include "RoomsWindowManager.h"
#include <trview.app/Windows/WindowIDs.h>
#include <trview.ui.render/Renderer.h>
#include <trview.graphics/DeviceWindow.h>

namespace trview
{
    RoomsWindowManager::RoomsWindowManager(graphics::Device& device, graphics::IShaderStorage& shader_storage, graphics::FontFactory& font_factory, const Window& window)
        : _device(device), _shader_storage(shader_storage), _font_factory(font_factory), MessageHandler(window)
    {
    }

    void RoomsWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND &&
            LOWORD(wParam) == ID_APP_WINDOWS_ROOMS ||
            LOWORD(wParam) == ID_APP_ACCEL_ROOMS_WINDOW)
        {
            create_window();
        }
    }

    void RoomsWindowManager::render(graphics::Device& device, bool vsync)
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

    void RoomsWindowManager::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
        for (auto& window : _windows)
        {
            window->set_items(_all_items);
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

    void RoomsWindowManager::set_rooms(const std::vector<Room*>& rooms)
    {
        _all_rooms = rooms;
        for (auto& window : _windows)
        {
            window->set_rooms(_all_rooms);
        }
    }

    void RoomsWindowManager::set_triggers(const std::vector<Trigger*>& triggers)
    {
        _all_triggers = triggers;
        for (auto& window : _windows)
        {
            window->set_triggers(_all_triggers);
        }
    }

    void RoomsWindowManager::create_window()
    {
        auto rooms_window = std::make_unique<RoomsWindow>(_device, _shader_storage, _font_factory, window());
        rooms_window->on_room_selected += on_room_selected;
        rooms_window->on_item_selected += on_item_selected;
        rooms_window->on_trigger_selected += on_trigger_selected;

        const auto window = rooms_window.get();
        _token_store += rooms_window->on_window_closed += [window, this]()
        {
            _closing_windows.push_back(window);
        };

        rooms_window->set_items(_all_items);
        rooms_window->set_triggers(_all_triggers);
        rooms_window->set_rooms(_all_rooms);
        rooms_window->set_current_room(_current_room);

        _windows.push_back(std::move(rooms_window));
    }
}