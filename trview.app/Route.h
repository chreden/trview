#pragma once

#include <cstdint>
#include <trview.graphics/Device.h>
#include "Mesh.h"

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
        explicit Route(const graphics::Device& device);

        /// Add a new waypoint to the end of the route.
        /// @param position The new waypoint.
        void add(const DirectX::SimpleMath::Vector3& position);

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

        /// Get the waypoint at the specified index.
        /// @param index The index to get.
        DirectX::SimpleMath::Vector3 waypoint(uint32_t index) const;
    private:
        std::vector<DirectX::SimpleMath::Vector3> _waypoints;
        std::unique_ptr<Mesh>                     _waypoint_mesh;
    };
}
