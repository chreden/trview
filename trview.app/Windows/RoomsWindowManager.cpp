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
            LOWORD(wParam) == ID_APP_WINDOWS_ITEMS ||
            LOWORD(wParam) == ID_APP_ACCEL_ITEM_WINDOW)
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

    void RoomsWindowManager::create_window()
    {
        auto rooms_window = std::make_unique<RoomsWindow>(_device, _shader_storage, _font_factory, window());

        const auto window = rooms_window.get();
        _token_store += rooms_window->on_window_closed += [window, this]()
        {
            _closing_windows.push_back(window);
        };

        _windows.push_back(std::move(rooms_window));
    }
}