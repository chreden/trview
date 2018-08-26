#include "ItemsWindowManager.h"
#include "WindowIDs.h"
#include <trview.ui.render/Renderer.h>
#include <trview.graphics/DeviceWindow.h>

namespace trview
{
    ItemsWindowManager::ItemsWindowManager(graphics::Device& device, graphics::IShaderStorage& shader_storage, graphics::FontFactory& font_factory, HWND window)
        : _device(device), _shader_storage(shader_storage), _font_factory(font_factory), MessageHandler(window)
    {
    }

    void ItemsWindowManager::process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_APP_WINDOWS_ITEMS)
        {
            create_window();
        }
    }

    void ItemsWindowManager::render(graphics::Device& device, bool vsync)
    {
        for (auto& window : _windows)
        {
            window->render(device, vsync);
        }
    }

    void ItemsWindowManager::create_window()
    {
        auto items_window = std::make_unique<ItemsWindow>(_device, _shader_storage, _font_factory, window());
        items_window->on_item_selected += on_item_selected;
        _windows.push_back(std::move(items_window));
    }

    void ItemsWindowManager::set_items(const std::vector<Item>& items)
    {
        for (auto& window : _windows)
        {
            window->set_items(items);
        }
    }

    void ItemsWindowManager::set_room(uint32_t room)
    {
        for (auto& window : _windows)
        {
            window->set_current_room(room);
        }
    }
}
