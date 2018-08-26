#pragma once

#include <vector>
#include <memory>

#include <trview.common/MessageHandler.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include <trview.common/TokenStore.h>
#include "ItemsWindow.h"

namespace trview
{
    class ItemsWindowManager final : public MessageHandler
    {
    public:
        explicit ItemsWindowManager(graphics::Device& device, graphics::IShaderStorage& shader_storage, graphics::FontFactory& font_factory, HWND window);
        virtual ~ItemsWindowManager() = default;
        virtual void process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam) override;

        Event<Item> on_item_selected;

        void render(graphics::Device& device, bool vsync);
        void set_items(const std::vector<Item>& items);
        void set_room(uint32_t room);
    private:
        void create_window();

        graphics::Device& _device;
        graphics::IShaderStorage& _shader_storage;
        graphics::FontFactory& _font_factory;
        std::vector<std::unique_ptr<ItemsWindow>> _windows;
        TokenStore _token_store;
    };
}

