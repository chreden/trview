/// @file RoomsWindowManager.h
/// @brief Controls and creates RoomsWindows.

#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <trview.app/Windows/IRoomsWindowManager.h>
#include <trview.common/MessageHandler.h>
#include <trview.common/TokenStore.h>
#include <trview.app/Elements/Item.h>

namespace trview
{
    class Room;
    class Shortcuts;

    /// Controls and creates RoomsWindows.
    class RoomsWindowManager final : public IRoomsWindowManager, public MessageHandler
    {
    public:
        /// Create an RoomsWindowManager.
        /// @param window The parent window of the rooms window.
        /// @param shortcuts The shortcuts instance.
        /// @param rooms_window_source The function to call to get a rooms window.
        explicit RoomsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const RoomsWindowSource& rooms_window_source);

        /// Destructor for the RoomsWindowManager.
        virtual ~RoomsWindowManager() = default;

        /// Handles a window message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Render all of the rooms windows.
        /// @param vsync Whether to use vsync.
        virtual void render(bool vsync) override;

        /// Set the items in the current level.
        virtual void set_items(const std::vector<Item>& items) override;

        /// Set the current room that the viewer is focusing on.
        /// @param room The current room.
        virtual void set_room(uint32_t room) override;

        /// Set the rooms to display in the window.
        /// @param rooms The rooms to show.
        virtual void set_rooms(const std::vector<Room*>& items) override;

        /// Set the item currently selected in the viewer.
        /// @param item The item currently selected.
        virtual void set_selected_item(const Item& item) override;

        /// Set the trigger currently selected in the viewer.
        /// @param trigger The trigger currently selected.
        virtual void set_selected_trigger(const Trigger* const trigger) override;

        /// Set the triggers in the level.
        /// @param triggers The triggers in the level.
        virtual void set_triggers(const std::vector<Trigger*>& triggers) override;

        /// Create a new rooms window.
        virtual void create_window() override;
    private:
        std::vector<std::shared_ptr<IRoomsWindow>> _windows;
        std::vector<std::weak_ptr<IRoomsWindow>> _closing_windows;
        std::vector<Item> _all_items;
        std::vector<Room*> _all_rooms;
        std::vector<Trigger*> _all_triggers;
        TokenStore _token_store;
        uint32_t _current_room;
        std::optional<const Trigger*> _selected_trigger;
        std::optional<Item> _selected_item;
        RoomsWindowSource _rooms_window_source;
    };
}

