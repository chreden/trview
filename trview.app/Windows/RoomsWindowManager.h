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

        /// Set the current room that the viewer is focusing on.
        /// @param room The current room.
        void set_room(uint32_t room);

        /// Set the rooms to display in the window.
        /// @param rooms The rooms to show.
        void set_rooms(const std::vector<Room*>& items);

        /// Set the item currently selected in the viewer.
        /// @param item The item currently selected.
        void set_selected_item(const Item& item);

        /// Set the trigger currently selected in the viewer.
        /// @param trigger The trigger currently selected.
        void set_selected_trigger(const Trigger* const trigger);

        /// Set the triggers in the level.
        /// @param triggers The triggers in the level.
        void set_triggers(const std::vector<Trigger*>& triggers);

        /// Create a new rooms window.
        void create_window();

        /// Event raised when the user has selected a room in the room window.
        Event<uint32_t> on_room_selected;

        /// Event raised when the user has selected an item in the room window.
        Event<Item> on_item_selected;

        /// Event raised when the user has selected a trigger in the room window.
        Event<Trigger*> on_trigger_selected;
    private:
        std::vector<std::unique_ptr<RoomsWindow>> _windows;
        std::vector<RoomsWindow*> _closing_windows;
        std::vector<Item> _all_items;
        std::vector<Room*> _all_rooms;
        std::vector<Trigger*> _all_triggers;
        graphics::Device& _device;
        graphics::IShaderStorage& _shader_storage;
        graphics::FontFactory& _font_factory;
        TokenStore _token_store;
        uint32_t _current_room;
        std::optional<const Trigger*> _selected_trigger;
        std::optional<Item> _selected_item;
    };
}

