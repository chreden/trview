#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <trview.app/Windows/ITriggersWindowManager.h>
#include <trview.common/MessageHandler.h>
#include <trview.common/TokenStore.h>
#include <trview.common/Windows/IShortcuts.h>

namespace trview
{
    /// Controls and creates TriggersWindows.
    class TriggersWindowManager final : public ITriggersWindowManager, public MessageHandler
    {
    public:
        /// Create an TriggersWindowManager.
        /// @param window The parent window of the triggers window.
        /// @param shortcuts The shortcuts instance to use.
        /// @param triggers_window_source Function to call to create a triggers window.
        explicit TriggersWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const ITriggersWindow::Source& triggers_window_source);

        /// Destructor for the TriggersWindowManager.
        virtual ~TriggersWindowManager() = default;

        /// Handles a window message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Render all of the triggers windows.
        /// @param vsync Whether to use vsync.
        virtual void render(bool vsync) override;

        /// Set the items to use in the windows.
        /// @param items The items in the level.
        virtual void set_items(const std::vector<Item>& items) override;

        /// Set the triggers to use in the windows.
        /// @param triggers The triggers in the level.
        virtual void set_triggers(const std::vector<Trigger*>& triggers) override;

        /// Set whether a trigger is visible.
        /// @param trigger The trigger.
        /// @param visible Whether the trigger is visible.
        virtual void set_trigger_visible(Trigger* trigger, bool visible) override;

        /// Set the current room to filter trigger windows.
        /// @param room The current room.
        virtual void set_room(uint32_t room) override;

        /// Set the currently selected trigger.
        /// @param item The selected trigger.
        virtual void set_selected_trigger(const Trigger* const trigger) override;

        /// Create a new triggers window.
        virtual std::weak_ptr<ITriggersWindow> create_window() override;
    private:
        std::vector<std::shared_ptr<ITriggersWindow>> _windows;
        std::vector<std::weak_ptr<ITriggersWindow>> _closing_windows;
        std::vector<Item> _items;
        std::vector<Trigger*> _triggers;
        uint32_t _current_room{ 0u };
        TokenStore _token_store;
        std::optional<const Trigger*> _selected_trigger;
        ITriggersWindow::Source _triggers_window_source;
    };
}
