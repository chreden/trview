
#pragma once

#include <trview.common/Event.h>
#include "../Routing/IWaypoint.h"
#include "../Elements/IItem.h"
#include "../Elements/Room.h"
#include <trview.common/Windows/IClipboard.h>
#include <trview.common/Windows/IDialogs.h>
#include <trview.common/IFiles.h>
#include "IRouteWindow.h"
#include <trview.common/Windows/IShell.h>
#include <trview.common/Messages/IMessageSystem.h>

namespace trview
{
    struct IRoute;

    class RouteWindow final : public IRouteWindow, public IRecipient, public std::enable_shared_from_this<IRecipient>
    {
    public:
        struct Names
        {
            static const std::string colour;
            static const inline std::string delete_waypoint = "Delete Waypoint";
            static const inline std::string attach_save = "Attach Save";
            static const inline std::string clear_save = "X";
            static const inline std::string notes = "Notes##notes";
            static const inline std::string waypoint_stats = "##waypointstats";
            static const inline std::string randomizer_flags = "Randomizer Flags";
            static const inline std::string waypoint_list_panel = "Waypoint List";
            static const inline std::string waypoint_details_panel = "Waypoint Details";
        };

        explicit RouteWindow(const std::shared_ptr<IClipboard>& clipboard, const std::shared_ptr<IDialogs>& dialogs,
            const std::shared_ptr<IFiles>& files, const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~RouteWindow() = default;
        virtual void render() override;
        virtual void set_route(const std::weak_ptr<IRoute>& route) override;
        void select_waypoint(const std::weak_ptr<IWaypoint>& waypoint) override;
        virtual void set_items(const std::vector<std::weak_ptr<IItem>>& items) override;
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual void focus() override;
        virtual void update(float delta) override;
        void receive_message(const Message& message) override;
    private:
        void load_randomiser_settings(IWaypoint& waypoint);
        void render_waypoint_list();
        void render_waypoint_details();
        bool render_route_window();
        void render_menu_bar();
        std::string waypoint_text(const IWaypoint& waypoint) const;

        std::weak_ptr<IRoute> _route;
        std::vector<std::weak_ptr<IItem>> _all_items;
        std::vector<std::weak_ptr<IRoom>> _all_rooms;
        std::vector<std::weak_ptr<ITrigger>> _all_triggers;
        IWaypoint::Type _selected_type{ IWaypoint::Type::Position };
        std::weak_ptr<IWaypoint> _selected_waypoint;
        std::shared_ptr<IClipboard> _clipboard;
        std::shared_ptr<IDialogs> _dialogs;
        std::shared_ptr<IFiles> _files;
        bool _randomizer_enabled{ false };
        std::optional<RandomizerSettings> _randomizer_settings;
        bool _scroll_to_waypoint{ false };
        std::optional<float> _tooltip_timer;
        bool _need_focus{ false };
        bool _show_settings{ false };
        std::weak_ptr<IMessageSystem> _messaging;
    };
}
