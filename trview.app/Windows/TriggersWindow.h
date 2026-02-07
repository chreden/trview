/// @file TriggersWindow.h
/// @brief Used to show and filter the triggers in the level.

#pragma once

#include <trview.common/Windows/IClipboard.h>
#include <trview.common/TokenStore.h>
#include <trview.common/Messages/IMessageSystem.h>
#include "../Filters/Filters.h"
#include "../Track/Track.h"

#include "IWindow.h"
#include "../Elements/IItem.h"
#include "../Elements/ITrigger.h"
#include "AutoHider.h"
#include "../Settings/UserSettings.h"

namespace trview
{
    class TriggersWindow final : public IWindow, public std::enable_shared_from_this<IRecipient>
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

        explicit TriggersWindow(const std::shared_ptr<IClipboard>& clipboard, const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~TriggersWindow() = default;
        virtual void render() override;
        void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers);
        void clear_selected_trigger();
        void set_current_room(const std::weak_ptr<IRoom>& room);
        virtual void set_number(int32_t number) override;
        void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger);
        void set_items(const std::vector<std::weak_ptr<IItem>>& items);
        void set_platform_and_version(const trlevel::PlatformAndVersion& platform_and_version);
        std::weak_ptr<ITrigger> selected_trigger() const;
        virtual void update(float delta) override;
        void receive_message(const Message& message) override;
        void initialise();
        std::string type() const override;
        std::string title() const override;
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
        std::optional<float> _tooltip_timer;
        std::vector<Command> _local_selected_trigger_commands;
        std::vector<std::weak_ptr<IItem>> _local_selected_trigger_trigger_triggerers;
        Filters _filters;
        bool _force_sort{ false };
        Track<Type::Room> _track;
        AutoHider _auto_hider;
        trlevel::PlatformAndVersion _platform_and_version;
        std::optional<UserSettings> _settings;
        bool _columns_set{ false };
        std::weak_ptr<IMessageSystem> _messaging;
        std::weak_ptr<ILevel> _level;
    };
}
