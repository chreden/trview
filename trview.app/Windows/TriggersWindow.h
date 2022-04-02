/// @file TriggersWindow.h
/// @brief Used to show and filter the triggers in the level.

#pragma once

#include <trview.common/Windows/IClipboard.h>
#include "../Filters/Filters.h"

#include "ITriggersWindow.h"
#include "../Elements/Item.h"
#include "../Elements/ITrigger.h"

namespace trview
{
    class TriggersWindow final : public ITriggersWindow
    {
    public:
        struct Names
        {
            static inline const std::string add_to_route = "Add to Route";
            static inline const std::string sync_trigger = "Sync";
            static inline const std::string track_room = "Track Room";
            static inline const std::string trigger_list_panel = "Trigger List";
            static inline const std::string triggers_list = "##triggerslist";
            static inline const std::string details_panel = "Trigger Details";
            static inline const std::string commands_list = "##commands";
            static inline const std::string trigger_stats = "##triggerstats";
            static inline const std::string command_filter = "##commandfilter";
        };

        explicit TriggersWindow(const std::shared_ptr<IClipboard>& clipboard);
        virtual ~TriggersWindow() = default;
        virtual void render() override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual void update_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual void clear_selected_trigger() override;
        virtual void set_current_room(uint32_t room) override;
        virtual void set_number(int32_t number) override;
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        virtual void set_items(const std::vector<Item>& items) override;
        virtual std::weak_ptr<ITrigger> selected_trigger() const override;
        virtual void update(float delta) override;
    private:
        void set_track_room(bool value);
        void set_sync_trigger(bool value);
        void render_triggers_list();
        void render_trigger_details();
        bool render_triggers_window();
        void set_local_selected_trigger(const std::weak_ptr<ITrigger>& trigger);
        void setup_filters();
        void filter_triggers();

        std::string _id{ "Triggers 0" };
        std::vector<Item> _all_items;
        std::vector<std::weak_ptr<ITrigger>> _all_triggers;
        std::vector<std::weak_ptr<ITrigger>> _filtered_triggers;

        /// Whether the trigger window is tracking the current room.
        bool _track_room{ false };
        /// The current room number selected for tracking.
        uint32_t _current_room{ 0u };
        /// Whether the room tracking filter has been applied.
        bool _filter_applied{ false };
        bool _sync_trigger{ true };
        std::vector<TriggerCommandType> _selected_commands;
        std::shared_ptr<IClipboard> _clipboard;
        std::vector<std::string> _all_commands;
        uint32_t _selected_command{ 0u };
        std::weak_ptr<ITrigger> _selected_trigger;
        std::weak_ptr<ITrigger> _global_selected_trigger;
        bool _scroll_to_trigger{ false };
        std::optional<float> _tooltip_timer;
        std::vector<Command> _local_selected_trigger_commands;
        Filters<ITrigger> _filters;
        bool _need_filtering{ true };
    };
}
