/// @file RoomsWindowManager.h
/// @brief Controls and creates RoomsWindows.

#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <trview.common/MessageHandler.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include <trview.common/TokenStore.h>
#include "RoomsWindow.h"
#include <trview.app/Elements/Item.h>

namespace trview
{
    class Room;

    /// Controls and creates RoomsWindows.
    class RoomsWindowManager final : public MessageHandler
    {
    public:
        /// Create an RoomsWindowManager.
        /// @param device The device to use for rooms windows.
        /// @param shader_storage The shader storage for rooms windows.
        /// @param font_factory The font_factory for rooms windows.
        /// @param window The parent window of the rooms window.
        explicit RoomsWindowManager(graphics::Device& device, graphics::IShaderStorage& shader_storage, graphics::FontFactory& font_factory, const Window& window);

        /// Destructor for the RoomsWindowManager.
        virtual ~RoomsWindowManager() = default;

        /// Handles a window message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Render all of the rooms windows.
        /// @param device The device to use to render.
        /// @param vsync Whether to use vsync.
        void render(graphics::Device& device, bool vsync);

        /// Set the items in the current level.
        void set_items(const std::vector<Item>& items);

        /// Set the rooms to display in the window.
        /// @param rooms The rooms to show.
        void set_rooms(const std::vector<Room*>& items);

        /// Create a new rooms window.
        void create_window();

        Event<uint32_t> on_room_selected;
    private:
        std::vector<std::unique_ptr<RoomsWindow>> _windows;
        std::vector<RoomsWindow*> _closing_windows;
        std::vector<Item> _all_items;
        std::vector<Room*> _all_rooms;
        graphics::Device& _device;
        graphics::IShaderStorage& _shader_storage;
        graphics::FontFactory& _font_factory;
        TokenStore _token_store;
    };
}

