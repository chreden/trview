/// @file ItemsWindow.h
/// @brief Used to show and filter the items in the level.

#pragma once

#include <trview.common/Windows/IClipboard.h>
#include "../Filters/Filters.h"

#include "IItemsWindow.h"
#include "../Elements/IItem.h"
#include "../Track/Track.h"
#include "ColumnSizer.h"
#include "AutoHider.h"

namespace trview
{
    /// Used to show and filter the items in the level.
    class ItemsWindow final : public IItemsWindow
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

        explicit ItemsWindow(const std::shared_ptr<IClipboard>& clipboard);
        virtual ~ItemsWindow() = default;
        void add_level(const std::weak_ptr<ILevel>& level) override;
        void set_filters(const std::weak_ptr<ILevel>& level, std::vector<Filters<IItem>::Filter> filters) override;
        virtual void render() override;
        void set_current_room(const std::weak_ptr<IRoom>& room) override;
        virtual void set_selected_item(const std::weak_ptr<IItem>& item) override;
        virtual void update(float delta) override;
        virtual void set_number(int32_t number) override;
        std::string name() const override;
    private:
        bool render_items_window();

        // Experimenting with multiple levels.
        struct SubWindow
        {
            Event<std::weak_ptr<IItem>> on_item_selected;
            Event<> on_scene_changed;
            Event<std::weak_ptr<ITrigger>> on_trigger_selected;
            Event<std::weak_ptr<IItem>> on_add_to_route;

            // To contain essentially the current contents of the items window, but nested.
            std::vector<std::weak_ptr<IItem>> _all_items;
            std::vector<std::weak_ptr<ITrigger>> _all_triggers;
            AutoHider _auto_hider;
            std::shared_ptr<IClipboard> _clipboard;
            ColumnSizer _column_sizer;
            std::weak_ptr<IRoom> _current_room;
            Filters<IItem> _filters;
            bool _force_sort{ false };
            std::weak_ptr<IItem> _global_selected_item;
            std::weak_ptr<ILevel> _level;
            trlevel::LevelVersion _level_version{ trlevel::LevelVersion::Unknown };
            std::function<bool(uint32_t)> _model_checker;
            bool _ng_plus{ false };
            bool _scroll_to_item{ false };
            std::weak_ptr<IItem> _selected_item;
            std::weak_ptr<ITrigger> _selected_trigger;
            bool _sync_item{ true };
            std::optional<float> _tooltip_timer;
            Track<Type::Room> _track;
            std::vector<std::weak_ptr<ITrigger>> _triggered_by;

            void calculate_column_widths();
            void render(int index);
            void render_items_list();
            void render_item_details();
            void set_current_room(const std::weak_ptr<IRoom>& room);
            void set_filters(const std::weak_ptr<ILevel>& level, std::vector<Filters<IItem>::Filter> filters);
            void set_items(const std::vector<std::weak_ptr<IItem>>& items);
            void set_local_selected_item(std::weak_ptr<IItem> item);
            void set_selected_item(const std::weak_ptr<IItem>& item);
            void set_sync_item(bool value);
            void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers);
            void setup_filters();
            void update(float delta);
        };

        std::string _id{ "Items 0" };
        std::vector<SubWindow> _sub_windows;
        std::shared_ptr<IClipboard> _clipboard;
    };
}
