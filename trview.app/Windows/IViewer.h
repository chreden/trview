#pragma once

#include <cstdint>
#include <SimpleMath.h>

#include <trview.common/Event.h>
#include "../Elements/IItem.h"
#include "../Elements/ITrigger.h"
#include "../Elements/ILevel.h"
#include "../Routing/IRoute.h"
#include "../Settings/UserSettings.h"

namespace trview
{
    struct IViewer
    {
        struct Options
        {
            inline static const std::string depth = "##DepthValue";
            inline static const std::string depth_enabled = "Depth";
            inline static const std::string flip = "Flip";
            inline static const std::string geometry = "Geometry";
            inline static const std::string highlight = "Highlight";
            inline static const std::string show_bounding_boxes = "Bounds";
            inline static const std::string triggers = "Triggers";
            inline static const std::string water = "Water";
            inline static const std::string wireframe = "Wireframe";
            inline static const std::string lights = "Lights";
            inline static const std::string items = "Items";
            inline static const std::string rooms = "Rooms";
            inline static const std::string camera_sinks = "Camera/Sink";
            inline static const std::string lighting = "Lighting";
            inline static const std::string animation = "Animation";
            inline static const std::string notes = "Notes";
            inline static const std::string sound_sources = "Sounds";
            inline static const std::string ng_plus = "NG+";
        };

        virtual ~IViewer() = 0;

        /// Event raised when the viewer wants to remove a waypoint.
        Event<uint32_t> on_waypoint_removed;

        /// Event raised when the viewer wants to add a waypoint.
        Event<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, std::weak_ptr<IRoom>, IWaypoint::Type, uint32_t> on_waypoint_added;

        Event<std::string, FontSetting> on_font;

        virtual std::weak_ptr<ICamera> camera() const = 0;

        virtual ICamera::Mode camera_mode() const = 0;

        /// Render the viewer.
        virtual void render() = 0;

        virtual void render_ui() = 0;

        virtual void present(bool vsync) = 0;

        virtual void open(const std::weak_ptr<ILevel>& level, ILevel::OpenMode open_mode) = 0;

        /// <summary>
        /// Select the specified room.
        /// </summary>
        /// <param name="room">The room to select</param>
        virtual void select_room(const std::weak_ptr<IRoom>& room) = 0;

        /// Select the specified waypoint
        /// @param index The waypoint to select.
        /// @remarks This will not raise the on_waypoint_selected event.
        virtual void select_waypoint(const std::weak_ptr<IWaypoint>& waypoint) = 0;

        virtual void set_camera_mode(ICamera::Mode camera_mode) = 0;

        /// Set whether the compass is visible.
        virtual void set_show_compass(bool value) = 0;

        /// Set whether the minimap is visible.
        virtual void set_show_minimap(bool value) = 0;

        /// Set whether the route is visible.
        virtual void set_show_route(bool value) = 0;

        /// Set whether the selection is visible.
        virtual void set_show_selection(bool value) = 0;

        /// Set whether the tools are visible.
        virtual void set_show_tools(bool value) = 0;

        /// Set whether the tooltip is visible.
        virtual void set_show_tooltip(bool value) = 0;

        /// Set whether the ui is visible.
        virtual void set_show_ui(bool value) = 0;

        virtual bool ui_input_active() const = 0;

        virtual DirectX::SimpleMath::Vector3 target() const = 0;

        virtual void set_target(const DirectX::SimpleMath::Vector3& target) = 0;

        virtual void select_sector(const std::weak_ptr<ISector>& sector) = 0;

        virtual void select_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) = 0;
        virtual void select_static_mesh(const std::weak_ptr<IStaticMesh>& static_mesh) = 0;
        virtual void select_flyby_node(const std::weak_ptr<IFlybyNode>& flyby_node) = 0;
        virtual std::weak_ptr<ILevel> level() const = 0;
    };
}
