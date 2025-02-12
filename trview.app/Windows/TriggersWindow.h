/// @file TriggersWindow.h
/// @brief Used to show and filter the triggers in the level.

#pragma once

#include <trview.common/Windows/IClipboard.h>
#include "../Filters/Filters.h"
#include "../Track/Track.h"

#include "ITriggersWindow.h"
#include "../Elements/IItem.h"
#include "../Elements/ITrigger.h"
#include "ColumnSizer.h"
#include "AutoHider.h"

namespace trview
{
    class TriggersWindow final : public ITriggersWindow
    {
    public:
        struct Names
        {
            static inline const std::string add_to_route = "Add to Route";
            static inline const std::string sync_trigger = "Sync";
            static inline const std::string trigger_list_panel = "Trigger List";
            static inline const std::string triggers_list = "##triggerslist";
            static inline const std::string details_panel = "Trigger Details";
            static inline const std::string commands_list = "##commands";
            static inline const std::string trigger_stats = "##triggerstats";
            static inline const std::string command_filter = "##commandfilter";
            static inline const std::string reset_colour = "Reset";
            static inline const std::string colour = "##colour";
        };

        explicit TriggersWindow(const std::shared_ptr<IClipboard>& clipboard);
        virtual ~TriggersWindow() = default;
        void add_level(const std::weak_ptr<ILevel>& level) override;
        void render() override;
        void set_current_room(const std::weak_ptr<IRoom>& room) override;
        void set_number(int32_t number) override;
        void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        void update(float delta) override;
    private:
        bool render_triggers_window();

        struct SubWindow
        {
            Event<std::weak_ptr<ITrigger>> on_add_to_route;
            Event<std::weak_ptr<ICameraSink>> on_camera_sink_selected;
            Event<std::weak_ptr<IItem>> on_item_selected;
            Event<> on_scene_changed;
            Event<std::weak_ptr<ITrigger>> on_trigger_selected;

            std::vector<std::weak_ptr<IItem>> _all_items;
            std::vector<std::weak_ptr<ITrigger>> _all_triggers;
            std::vector<std::weak_ptr<ITrigger>> _filtered_triggers;

            std::shared_ptr<IClipboard> _clipboard;
            std::weak_ptr<ILevel> _level;
            std::weak_ptr<IRoom> _current_room;
            /// Whether the room tracking filter has been applied.
            bool _filter_applied{ false };
            bool _sync_trigger{ true };
            std::vector<TriggerCommandType> _selected_commands;

            struct VirtualCommand
            {
                std::string        name;
                TriggerCommandType type;
                bool operator == (const VirtualCommand& other) const noexcept;
            };
            std::vector<VirtualCommand> _all_commands;
            std::optional<VirtualCommand> _selected_command;
            std::weak_ptr<ITrigger> _selected_trigger;
            std::weak_ptr<ITrigger> _global_selected_trigger;
            bool _scroll_to_trigger{ false };
            std::optional<float> _tooltip_timer;
            std::vector<Command> _local_selected_trigger_commands;
            Filters<ITrigger> _filters;
            bool _need_filtering{ true };
            ColumnSizer _column_sizer;
            bool _force_sort{ false };
            Track<Type::Room> _track;
            AutoHider _auto_hider;

            void calculate_column_widths();
            void filter_triggers();
            std::optional<int> index_of_selected() const;
            void render(int index);
            void render_triggers_list();
            void render_trigger_details();
            void set_current_room(const std::weak_ptr<IRoom>& room);
            void set_items(const std::vector<std::weak_ptr<IItem>>& items);
            void set_local_selected_trigger(const std::weak_ptr<ITrigger>& trigger);
            void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger);
            void set_sync_trigger(bool value);
            void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers);
            void setup_filters();
            void update(float delta);
        };

        std::string _id{ "Triggers 0" };
        std::vector<SubWindow> _sub_windows;
        std::shared_ptr<IClipboard> _clipboard;
    };
}
