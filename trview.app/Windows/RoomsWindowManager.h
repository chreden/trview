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
#include <trview.common/Windows/IShortcuts.h>

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
        explicit RoomsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IRoomsWindow::Source& rooms_window_source);
        virtual ~RoomsWindowManager() = default;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual void render(bool vsync) override;
        std::weak_ptr<ITrigger> selected_trigger() const;
        virtual void set_items(const std::vector<Item>& items) override;
        virtual void set_level_version(trlevel::LevelVersion version) override;
        virtual void set_room(uint32_t room) override;
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& items) override;
        virtual void set_selected_item(const Item& item) override;
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual std::weak_ptr<IRoomsWindow> create_window() override;
        virtual void update(float delta) override;
    private:
        std::vector<std::shared_ptr<IRoomsWindow>> _windows;
        std::vector<std::weak_ptr<IRoomsWindow>> _closing_windows;
        std::vector<Item> _all_items;
        std::vector<std::weak_ptr<IRoom>> _all_rooms;
        std::vector<std::weak_ptr<ITrigger>> _all_triggers;
        TokenStore _token_store;
        uint32_t _current_room{ 0u };
        std::weak_ptr<ITrigger> _selected_trigger;
        std::optional<Item> _selected_item;
        IRoomsWindow::Source _rooms_window_source;
        trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Unknown };
    };
}

