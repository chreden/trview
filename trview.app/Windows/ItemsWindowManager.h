/// @file ItemsWindowManager.h
/// @brief Controls and creates ItemsWindows.

#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <trview.common/MessageHandler.h>
#include <trview.common/Windows/Shortcuts.h>
#include "../Settings/UserSettings.h"
#include "IItemsWindowManager.h"
#include "WindowManager.h"

namespace trview
{
    /// Controls and creates ItemsWindows.
    class ItemsWindowManager final : public IItemsWindowManager, public WindowManager<IItemsWindow>, public MessageHandler
    {
    public:
        /// Create an TriggersWindowManager.
        /// @param window The parent window of the triggers window.
        /// @param shortcuts The shortcuts instance to use.
        /// @param items_window_source Function to call to create a triggers window.
        explicit ItemsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IItemsWindow::Source& items_window_source);
        virtual ~ItemsWindowManager() = default;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual void render() override;
        virtual void set_items(const std::vector<std::weak_ptr<IItem>>& items) override;
        virtual void set_level_version(trlevel::LevelVersion version) override;
        virtual void set_model_checker(const std::function<bool(uint32_t)>& checker) override;
        void set_ng_plus(bool value) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        void set_room(const std::weak_ptr<IRoom>& room) override;
        virtual void set_selected_item(const std::weak_ptr<IItem>& item) override;
        virtual std::weak_ptr<IItemsWindow> create_window() override;
        virtual void update(float delta) override;
        std::vector<std::weak_ptr<IItemsWindow>> windows() const override;
        void set_settings(const UserSettings& settings) override;
    private:
        std::vector<std::weak_ptr<IItem>> _items;
        std::vector<std::weak_ptr<ITrigger>> _triggers;
        std::weak_ptr<IRoom> _current_room;
        std::weak_ptr<IItem> _selected_item;
        IItemsWindow::Source _items_window_source;
        trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Unknown };
        std::function<bool(uint32_t)> _model_checker;
        bool _ng_plus{ false };
        UserSettings _settings;
    };
}
