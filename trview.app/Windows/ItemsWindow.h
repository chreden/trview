/// @file ItemsWindow.h
/// @brief Used to show and filter the items in the level.

#pragma once

#include <trview.common/TokenStore.h>
#include <trview.common/Windows/IClipboard.h>
#include <trview.common/Messages/IMessageSystem.h>
#include "../Filters/Filters.h"

#include "IItemsWindow.h"
#include "../Elements/IItem.h"
#include "../Track/Track.h"
#include "AutoHider.h"
#include "../Settings/UserSettings.h"

namespace trview
{
    /// Used to show and filter the items in the level.
    class ItemsWindow final : public IItemsWindow, public IRecipient, public std::enable_shared_from_this<IRecipient>
    {
    public:
        struct Names
        {
            static inline const std::string add_to_route_button = "Add to Route";
            static inline const std::string sync_item = "Sync";
            static inline const std::string items_list = "##itemslist";
            static inline const std::string item_list_panel = "Item List";
            static inline const std::string details_panel = "Item Details";
            static inline const std::string triggers_list = "##triggeredby";
            static inline const std::string item_stats = "##itemstats";
            static inline const std::string auto_hide = "Auto-Hide";
        };

        explicit ItemsWindow(const std::shared_ptr<IClipboard>& clipboard, const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~ItemsWindow() = default;
        void set_filters(std::vector<Filters<IItem>::Filter> filters) override;
        virtual void render() override;
        virtual void set_items(const std::vector<std::weak_ptr<IItem>>& items) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual void clear_selected_item() override;
        void set_current_room(const std::weak_ptr<IRoom>& room) override;
        virtual void set_selected_item(const std::weak_ptr<IItem>& item) override;
        virtual std::weak_ptr<IItem> selected_item() const override;
        virtual void update(float delta) override;
        virtual void set_number(int32_t number) override;
        virtual void set_level_version(trlevel::LevelVersion version) override;
        virtual void set_model_checker(const std::function<bool(uint32_t)>& checker) override;
        void set_ng_plus(bool value) override;
        std::string name() const override;
        void receive_message(const Message& message) override;
    private:
        void set_sync_item(bool value);
        void render_items_list();
        void render_item_details();
        bool render_items_window();
        void set_local_selected_item(std::weak_ptr<IItem> item);
        void setup_filters();
        void render_trigger_references();

        std::string _id{ "Items 0" };
        std::vector<std::weak_ptr<IItem>> _all_items;
        std::vector<std::weak_ptr<ITrigger>> _all_triggers;
        std::weak_ptr<IRoom> _current_room;
        bool _sync_item{ true };
        std::shared_ptr<IClipboard> _clipboard;
        std::unordered_map<std::string, std::string> _tips;
        std::optional<float> _tooltip_timer;
        std::weak_ptr<ITrigger> _selected_trigger;
        std::weak_ptr<IItem> _selected_item;
        std::weak_ptr<IItem> _global_selected_item;
        std::vector<std::weak_ptr<ITrigger>> _triggered_by;

        Filters<IItem> _filters;
        trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Unknown };
        std::function<bool(uint32_t)> _model_checker;
        bool _force_sort{ false };
        Track<Type::Room> _track;

        bool _ng_plus{ false };
        AutoHider _auto_hider;
        std::optional<UserSettings> _settings;
        TokenStore _token_store;
        bool _columns_set{ false };

        std::weak_ptr<IMessageSystem> _messaging;
    };
}
