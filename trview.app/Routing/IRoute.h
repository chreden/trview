#pragma once

#include <cstdint>
#include <trview.app/Routing/Waypoint.h>

namespace trview
{
    struct IRoute
    {
        using Source = std::function<std::unique_ptr<IRoute>()>;

        virtual ~IRoute() = 0;
        
        /// Add a new waypoint to the end of the route.
        /// @param position The new waypoint.
        /// @param room The room the waypoint is in.
        virtual void add(const DirectX::SimpleMath::Vector3& position, uint32_t room) = 0;

        /// Add a new waypoint to the end of the route.
        /// @param position The position of the waypoint in the world.
        /// @param room The room that waypoint is in.
        /// @param type The type of the waypoint.
        /// @param type_index The index of the referred to entity or trigger.
        virtual void add(const DirectX::SimpleMath::Vector3& position, uint32_t room, Waypoint::Type type, uint32_t type_index) = 0;

        /// Remove all of the waypoints from the route.
        virtual void clear() = 0;

        /// Get the colour of the route.
        virtual Colour colour() const = 0;

        /// Insert the new waypoint into the route.
        /// @param position The new waypoint.
        /// @param room The room that the waypoint is in.
        /// @param index The index in the route list to put the waypoint.
        virtual void insert(const DirectX::SimpleMath::Vector3& position, uint32_t room, uint32_t index) = 0;

        /// Insert the new waypoint into the route based on the currently selected waypoint.
        /// @param position The new waypoint.
        /// @param room The room that the waypoint is in.
        /// @return The index of the new waypoint.
        virtual uint32_t insert(const DirectX::SimpleMath::Vector3& position, uint32_t room) = 0;

        /// Insert a new non-positional waypoint.
        /// @param position The position of the waypoint in the world.
        /// @param room The room that the waypoint is in.
        /// @param type The type of waypoint.
        /// @param index The index in the route list to put the waypoint.
        /// @param type_index The index of the trigger or entity to reference.
        virtual void insert(const DirectX::SimpleMath::Vector3& position, uint32_t room, uint32_t index, Waypoint::Type type, uint32_t type_index) = 0;

        /// Insert a new non-positional waypoint based on the currently selected waypoint.
        /// @param position The position of the waypoint in the world.
        /// @param room The room that the waypoint is in.
        /// @param type The type of waypoint.
        /// @param type_index The index of the trigger or entity to reference.
        /// @return The index of the new waypoint.
        virtual uint32_t insert(const DirectX::SimpleMath::Vector3& position, uint32_t room, Waypoint::Type type, uint32_t type_index) = 0;

        /// Pick against the waypoints in the route.
        /// @param position The position of the camera.
        /// @param direction The direction of the ray.
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;

        /// Remove the waypoint at the specified index.
        /// @param index The index of the waypoint to remove.
        virtual void remove(uint32_t index) = 0;

        /// Render the route.
        /// @param camera The camera to use to render.
        /// @param texture_storage Texture storage for the mesh.
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage) = 0;

        /// Get the index of the currently selected waypoint.
        virtual uint32_t selected_waypoint() const = 0;

        /// Set the specified waypoint index to be the selected waypoint.
        /// @param index The index to select.
        virtual void select_waypoint(uint32_t index) = 0;

        /// Set the colour for the route.
        /// @param colour The colour to use.
        virtual void set_colour(const Colour& colour) = 0;

        /// Get the waypoint at the specified index.
        /// @param index The index to get.
        virtual const Waypoint& waypoint(uint32_t index) const = 0;

        /// Get the waypoint at the specified index.
        /// @param index The index to get.
        virtual Waypoint& waypoint(uint32_t index) = 0;

        /// Get the number of waypoints in the route.
        virtual uint32_t waypoints() const = 0;
    };
}