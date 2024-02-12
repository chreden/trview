#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <trview.common/MessageHandler.h>
#include <trview.common/Windows/IShortcuts.h>
#include "ITriggersWindowManager.h"
#include "WindowManager.h"

namespace trview
{
    /// Controls and creates TriggersWindows.
    class TriggersWindowManager final : public ITriggersWindowManager, public WindowManager<ITriggersWindow>, public MessageHandler
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
        virtual void set_items(const std::vector<std::weak_ptr<IItem>>& items) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        void set_room(const std::weak_ptr<IRoom>& room) override;
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        virtual std::weak_ptr<ITriggersWindow> create_window() override;
        virtual void update(float delta) override;
    private:
        std::vector<std::weak_ptr<IItem>> _items;
        std::vector<std::weak_ptr<ITrigger>> _triggers;
        std::weak_ptr<IRoom> _current_room;
        std::weak_ptr<ITrigger> _selected_trigger;
        ITriggersWindow::Source _triggers_window_source;
    };
}
