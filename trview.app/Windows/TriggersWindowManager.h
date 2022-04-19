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
        virtual ~TriggersWindowManager() = default;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual void render() override;
        const std::weak_ptr<ITrigger> selected_trigger() const;
        virtual void set_items(const std::vector<Item>& items) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual void set_trigger_visible(const std::weak_ptr<ITrigger>& trigger, bool visible) override;
        virtual void set_room(uint32_t room) override;
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        virtual std::weak_ptr<ITriggersWindow> create_window() override;
        virtual void update(float delta) override;
    private:
        int32_t next_id() const;
        std::unordered_map<int32_t, std::shared_ptr<ITriggersWindow>> _windows;
        std::vector<int32_t> _closing_windows;
        std::vector<Item> _items;
        std::vector<std::weak_ptr<ITrigger>> _triggers;
        uint32_t _current_room{ 0u };
        TokenStore _token_store;
        std::weak_ptr<ITrigger> _selected_trigger;
        ITriggersWindow::Source _triggers_window_source;
    };
}
