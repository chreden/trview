#pragma once

#include <cstdint>
#include <trview.app/Routing/IWaypoint.h>
#include <trview.app/Geometry/PickResult.h>

namespace trview
{
    /// <summary>
    /// A route is a series of waypoints with notes.
    /// </summary>
    struct IRoute
    {
        using Source = std::function<std::shared_ptr<IRoute>()>;
        virtual ~IRoute() = 0;
        /// <summary>
        /// Add a new waypoint to the end of the route.
        /// </summary>
        /// <param name="position">The new waypoint.</param>
        /// <param name="normal">The normal to align the waypoint to.</param>
        /// <param name="room">The room the waypoint is in.</param>
        virtual void add(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room) = 0;
        /// <summary>
        /// Add a new waypoint to the end of the route.
        /// </summary>
        /// <param name="position">The position of the waypoint in the world.</param>
        /// <param name="normal">The normal to align the waypoint to.</param>
        /// <param name="room">The room that waypoint is in.</param>
        /// <param name="type">The type of the waypoint.</param>
        /// <param name="type_index">The index of the referred to entity or trigger.</param>
        virtual void add(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t type_index) = 0;
        /// <summary>
        /// Remove all of the waypoints from the route.
        /// </summary>
        virtual void clear() = 0;
        /// <summary>
        /// Get the colour of the route.
        /// </summary>
        /// <returns>The colour of the route.</returns>
        virtual Colour colour() const = 0;
        /// <summary>
        /// Insert the new waypoint into the route.
        /// </summary>
        /// <param name="position">The new waypoint.</param>
        /// <param name="normal">The normal to align the waypoint to.</param>
        /// <param name="room">The room that the waypoint is in.</param>
        /// <param name="index">The index in the route list to put the waypoint.</param>
        virtual void insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, uint32_t index) = 0;
        /// <summary>
        /// Insert the new waypoint into the route based on the currently selected waypoint.
        /// </summary>
        /// <param name="position">The new waypoint.</param>
        /// <param name="normal">The normal to align the waypoint to.</param>
        /// <param name="room">The room that the waypoint is in.</param>
        /// <returns>The index of the new waypoint.</returns>
        virtual uint32_t insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room) = 0;
        /// <summary>
        /// Insert a new non-positional waypoint.
        /// </summary>
        /// <param name="position">The position of the waypoint in the world.</param>
        /// <param name="normal">The normal to align the waypoint to.</param>
        /// <param name="room">The room that the waypoint is in.</param>
        /// <param name="index">The index in the route list to put the waypoint.</param>
        /// <param name="type">The type of waypoint.</param>
        /// <param name="type_index">The index of the trigger or entity to reference.</param>
        virtual void insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, uint32_t index, IWaypoint::Type type, uint32_t type_index) = 0;
        /// <summary>
        /// Insert a new non-positional waypoint based on the currently selected waypoint.
        /// </summary>
        /// <param name="position">The position of the waypoint in the world.</param>
        /// <param name="normal">The normal to align the waypoint to.</param>
        /// <param name="room">The room that the waypoint is in.</param>
        /// <param name="type">The type of waypoint.</param>
        /// <param name="type_index">The index of the trigger or entity to reference.</param>
        /// <returns>The index of the new waypoint.</returns>
        virtual uint32_t insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t type_index) = 0;
        /// <summary>
        /// Determines whether the route has any unsaved changes.
        /// </summary>
        /// <returns>True if there are unsaved changes.</returns>
        virtual bool is_unsaved() const = 0;
        /// <summary>
        /// Move a waypoint from one index to another.
        /// </summary>
        /// <param name="from">The source index.</param>
        /// <param name="to">Destination index.</param>
        virtual void move(int32_t from, int32_t to) = 0;
        /// <summary>
        /// Pick against the waypoints in the route.
        /// </summary>
        /// <param name="position">The position of the camera.</param>
        /// <param name="direction">The direction of the ray.</param>
        /// <returns>The pcik result.</returns>
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;
        /// <summary>
        /// Remove the waypoint at the specified index.
        /// </summary>
        /// <param name="index">The index of the waypoint to remove.</param>
        virtual void remove(uint32_t index) = 0;
        /// <summary>
        /// Render the route.
        /// </summary>
        /// <param name="camera">The camera to use to render.</param>
        /// <param name="texture_storage">Texture storage for the mesh.</param>
        /// <param name="show_selection">Whether to show the selection outline.</param>
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage, bool show_selection) = 0;
        /// <summary>
        /// Get the index of the currently selected waypoint.
        /// </summary>
        /// <returns>The index of the currently selected waypoint.</returns>
        virtual uint32_t selected_waypoint() const = 0;
        /// <summary>
        /// Set the specified waypoint index to be the selected waypoint.
        /// </summary>
        /// <param name="index">The index to select.</param>
        virtual void select_waypoint(uint32_t index) = 0;
        /// <summary>
        /// Set the colour for the route.
        /// </summary>
        /// <param name="colour">The colour to use.</param>
        virtual void set_colour(const Colour& colour) = 0;
        /// <summary>
        /// Set whether randomizer tools are enabled.
        /// </summary>
        /// <param name="enabled">Whether randomizer tools are enabled.</param>
        virtual void set_randomizer_enabled(bool enabled) = 0;
        /// <summary>
        /// Set the colour for the stick.
        /// </summary>
        /// <param name="colour">The colour to use.</param>
        virtual void set_stick_colour(const Colour& colour) = 0;
        /// <summary>
        /// Set whether the route has unsaved changes.
        /// </summary>
        /// <param name="value">Whether the route has unsaved changes.</param>
        virtual void set_unsaved(bool value) = 0;
        /// <summary>
        /// Get the colour to use for the stick.
        /// </summary>
        virtual Colour stick_colour() const = 0;
        /// <summary>
        /// Get the waypoint at the specified index.
        /// </summary>
        /// <param name="index">The index to get.</param>
        /// <returns>The waypoint.</returns>
        virtual const IWaypoint& waypoint(uint32_t index) const = 0;
        /// <summary>
        /// Get the waypoint at the specified index.
        /// </summary>
        /// <param name="index">The index to get.</param>
        /// <returns>The waypoint.</returns>
        virtual IWaypoint& waypoint(uint32_t index) = 0;
        /// <summary>
        /// Get the number of waypoints in the route.
        /// </summary>
        /// <returns>The number of waypoints in the route.</returns>
        virtual uint32_t waypoints() const = 0;
    };
}