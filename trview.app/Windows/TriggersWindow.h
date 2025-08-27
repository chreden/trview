/// @file TriggersWindow.h
/// @brief Used to show and filter the triggers in the level.

#pragma once

#include <trview.common/Windows/IClipboard.h>
#include <trview.common/TokenStore.h>
#include "../Filters/Filters.h"
#include "../Track/Track.h"

#include "ITriggersWindow.h"
#include "../Elements/IItem.h"
#include "../Elements/ITrigger.h"
#include "AutoHider.h"
#include "../Settings/UserSettings.h"

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
        virtual void render() override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual void clear_selected_trigger() override;
        void set_current_room(const std::weak_ptr<IRoom>& room) override;
        virtual void set_number(int32_t number) override;
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        virtual void set_items(const std::vector<std::weak_ptr<IItem>>& items) override;
        void set_platform_and_version(const trlevel::PlatformAndVersion& platform_and_version) override;
        virtual std::weak_ptr<ITrigger> selected_trigger() const override;
        virtual void update(float delta) override;
        void set_settings(const UserSettings& settings) override;
    private:
        void set_sync_trigger(bool value);
        void render_triggers_list();
        void render_trigger_details();
        bool render_triggers_window();
        void set_local_selected_trigger(const std::weak_ptr<ITrigger>& trigger);
        void setup_filters();
        void find_trigger_triggerers();

        std::string _id{ "Triggers 0" };
        std::vector<std::weak_ptr<IItem>> _all_items;
        std::vector<std::weak_ptr<ITrigger>> _all_triggers;

        TokenStore _token_store;
        std::weak_ptr<IRoom> _current_room;
        /// Whether the room tracking filter has been applied.
        bool _sync_trigger{ true };
        std::vector<TriggerCommandType> _selected_commands;
        std::shared_ptr<IClipboard> _clipboard;

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
        std::vector<std::weak_ptr<IItem>> _local_selected_trigger_trigger_triggerers;
        Filters<ITrigger> _filters;
        bool _force_sort{ false };
        Track<Type::Room> _track;
        AutoHider _auto_hider;
        trlevel::PlatformAndVersion _platform_and_version;
        UserSettings _settings;
        bool _columns_set{ false };
    };
}
