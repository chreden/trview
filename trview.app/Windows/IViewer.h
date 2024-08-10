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
            inline static const std::string notes = "Notes";
        };

        virtual ~IViewer() = 0;

        /// Event raised when the user settings have changed.
        /// @remarks The settings is passed as a parameter to the listener functions.
        Event<UserSettings> on_settings;

        /// Event raised when the viwer wants to select an item.
        Event<std::weak_ptr<IItem>> on_item_selected;

        /// Event raised when the viewer wants to change the visibility of an item.
        Event<std::weak_ptr<IItem>, bool> on_item_visibility;

        /// Event raised when the viewer wants to select a room.
        Event<std::weak_ptr<IRoom>> on_room_selected;

        /// Event raised when the viewer wants to select a trigger.
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;

        /// Event raised when the viewer wants to change the visibility of a trigger.
        Event<std::weak_ptr<ITrigger>, bool> on_trigger_visibility;

        /// <summary>
        /// Event raised when the viewer wants to select a light.
        /// </summary>
        Event<std::weak_ptr<ILight>> on_light_selected;

        /// <summary>
        /// Event raised when the viewer wants to change the visibility of a light.
        /// </summary>
        Event<std::weak_ptr<ILight>, bool> on_light_visibility;

        Event<std::weak_ptr<IRoom>, bool> on_room_visibility;

        Event<std::weak_ptr<ICameraSink>, bool> on_camera_sink_visibility;

        /// Event raised when the viewer wants to select a waypoint.
        Event<std::weak_ptr<IWaypoint>> on_waypoint_selected;

        /// Event raised when the viewer wants to remove a waypoint.
        Event<uint32_t> on_waypoint_removed;

        /// Event raised when the viewer wants to add a waypoint.
        Event<DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3, std::weak_ptr<IRoom>, IWaypoint::Type, uint32_t> on_waypoint_added;

        Event<std::weak_ptr<ICameraSink>> on_camera_sink_selected;

        Event<std::string, FontSetting> on_font;

        Event<std::weak_ptr<IStaticMesh>> on_static_mesh_selected;

        virtual std::weak_ptr<ICamera> camera() const = 0;

        virtual ICamera::Mode camera_mode() const = 0;

        /// Render the viewer.
        virtual void render() = 0;

        virtual void render_ui() = 0;

        virtual void present(bool vsync) = 0;

        virtual void open(const std::weak_ptr<ILevel>& level, ILevel::OpenMode open_mode) = 0;

        virtual void set_settings(const UserSettings& settings) = 0;

        /// Select the specified item.
        /// @param item The item to select.
        /// @remarks This will not raise the on_item_selected event.
        virtual void select_item(const std::weak_ptr<IItem>& item) = 0;

        /// <summary>
        /// Select the specified room.
        /// </summary>
        /// <param name="room">The room to select</param>
        /// <remarks>This will not raise the on_room_selected event.</remarks>
        virtual void select_room(const std::weak_ptr<IRoom>& room) = 0;

        /// Select the specified trigger.
        /// @param trigger The trigger to select.
        /// @remarks This will not raise the on_trigger_selected event.
        virtual void select_trigger(const std::weak_ptr<ITrigger>& trigger) = 0;

        /// Select the specified waypoint
        /// @param index The waypoint to select.
        /// @remarks This will not raise the on_waypoint_selected event.
        virtual void select_waypoint(const std::weak_ptr<IWaypoint>& waypoint) = 0;

        virtual void set_camera_mode(ICamera::Mode camera_mode) = 0;

        /// Set the current route.
        /// @param route The new route.
        virtual void set_route(const std::shared_ptr<IRoute>& route) = 0;

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

        virtual void select_light(const std::weak_ptr<ILight>& light) = 0;

        virtual DirectX::SimpleMath::Vector3 target() const = 0;

        virtual void set_target(const DirectX::SimpleMath::Vector3& target) = 0;

        virtual void select_sector(const std::weak_ptr<ISector>& sector) = 0;
        virtual void set_scene_changed() = 0;

        virtual void select_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) = 0;
        virtual void select_static_mesh(const std::weak_ptr<IStaticMesh>& static_mesh) = 0;
    };
}
