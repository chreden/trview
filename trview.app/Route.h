#pragma once

#include <cstdint>
#include <trview.graphics/Device.h>
#include <trview.graphics/IShaderStorage.h>
#include "Mesh.h"
#include "SelectionRenderer.h"
#include "Waypoint.h"

namespace trview
{
    struct ICamera;
    struct ILevelTextureStorage;

    /// A series of waypoints.
    class Route final
    {
    public:
        /// Create a route.
        /// @param device The device to use.
        explicit Route(const graphics::Device& device, const graphics::IShaderStorage& shader_storage);

        /// Add a new waypoint to the end of the route.
        /// @param position The new waypoint.
        /// @param room The room the waypoint is in.
        void add(const DirectX::SimpleMath::Vector3& position, uint32_t room);

        /// Add a new waypoint to the end of the route.
        /// @param position The position of the waypoint in the world.
        /// @param room The room that waypoint is in.
        /// @param type The type of the waypoint.
        /// @param type_index The index of the referred to entity or trigger.
        void add(const DirectX::SimpleMath::Vector3& position, uint32_t room, Waypoint::Type type, uint32_t type_index);

        /// Remove all of the waypoints from the route.
        void clear();

        /// Insert the new waypoint into the route.
        /// @param position The new waypoint.
        /// @param room The room that the waypoint is in.
        /// @param index The index in the route list to put the waypoint.
        /// @param room The room
        void insert(const DirectX::SimpleMath::Vector3& position, uint32_t room, uint32_t index);

        /// Insert a new non-positional waypoint.
        /// @param position The position of the waypoint in the world.
        /// @param room The room that the waypoint is in.
        /// @param type The type of waypoint.
        /// @param index The index in the route list to put the waypoint.
        /// @param type_index The index of the trigger or entity to reference.
        void insert(const DirectX::SimpleMath::Vector3& position, uint32_t room, uint32_t index, Waypoint::Type type, uint32_t type_index);

        /// Pick against the waypoints in the route.
        /// @param position The position of the camera.
        /// @param direction The direction of the ray.
        PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const;

        /// Remove the waypoint at the specified index.
        /// @param index The index of the waypoint to remove.
        void remove(uint32_t index);

        /// Render the route.
        /// @param device The device to use to render.
        /// @param camera The camera to use to render.
        /// @param texture_storage Texture storage for the mesh.
        void render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage);

        /// Get the index of the currently selected waypoint.
        uint32_t selected_waypoint() const;

        /// Set the specified waypoint index to be the selected waypoint.
        /// @param index The index to select.
        void select_waypoint(uint32_t index);

        /// Get the waypoint at the specified index.
        /// @param index The index to get.
        const Waypoint& waypoint(uint32_t index) const;

        /// Get the number of waypoints in the route.
        uint32_t waypoints() const;
    private:
        std::vector<Waypoint> _waypoints;
        std::unique_ptr<Mesh> _waypoint_mesh;
        SelectionRenderer     _selection_renderer;
        uint32_t              _selected_index{ 0u };
    };
}
