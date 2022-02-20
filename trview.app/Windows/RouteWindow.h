
#pragma once

#include <trview.common/Event.h>
#include <trview.app/Routing/IWaypoint.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/Room.h>
#include <trview.common/Windows/IClipboard.h>
#include <trview.common/Windows/IDialogs.h>
#include <trview.common/IFiles.h>
#include "IRouteWindow.h"
#include <trview.common/Windows/IShell.h>

namespace trview
{
    struct IRoute;

    class RouteWindow final : public IRouteWindow
    {
    public:
        struct Names
        {
            static const std::string colour;
            static const std::string waypoints;
            static const std::string delete_waypoint;
            static const inline std::string export_button = "Export";
            static const inline std::string import_button = "Import";
            static const std::string clear_save;
            static const std::string notes_area;
            static const std::string select_save_button;
            static const std::string waypoint_stats;
            static const std::string randomizer_group;
            static const std::string randomizer_area;
            static const inline std::string waypoint_list_panel = "Waypoint List";
            static const inline std::string waypoint_details_panel = "Waypoint Details";
        };

        /// Create a route window as a child of the specified window.
        /// @param device The graphics device
        /// @param renderer_source The function to call to get a renderer.
        /// @param parent The parent window.
        explicit RouteWindow(const trview::Window& parent, const std::shared_ptr<IClipboard>& clipboard, const std::shared_ptr<IDialogs>& dialogs,
            const std::shared_ptr<IFiles>& files, const std::shared_ptr<IShell>& shell);
        virtual ~RouteWindow() = default;
        virtual void render() override;
        virtual void set_route(IRoute* route) override;
        virtual void select_waypoint(uint32_t index) override;
        virtual void set_items(const std::vector<Item>& items) override;
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual void focus() override;
        virtual void update(float delta) override;
        virtual void set_randomizer_enabled(bool value) override;
        virtual void set_randomizer_settings(const RandomizerSettings& settings) override;
    private:
        void load_randomiser_settings(IWaypoint& waypoint);
        void render_waypoint_list();
        void render_waypoint_details();
        bool render_route_window();

        IRoute* _route{ nullptr };
        std::vector<Item> _all_items;
        std::vector<std::weak_ptr<IRoom>> _all_rooms;
        std::vector<std::weak_ptr<ITrigger>> _all_triggers;
        IWaypoint::Type _selected_type{ IWaypoint::Type::Position };
        uint32_t       _selected_index{ 0u };
        std::shared_ptr<IClipboard> _clipboard;
        std::shared_ptr<IDialogs> _dialogs;
        std::shared_ptr<IFiles> _files;
        bool _randomizer_enabled{ false };
        RandomizerSettings _randomizer_settings;
        trview::Window _window;
        bool _scroll_to_trigger{ false };
    };
}
