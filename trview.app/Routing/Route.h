#pragma once

#include <trview.app/Geometry/Mesh.h>
#include <trview.app/Graphics/ISelectionRenderer.h>
#include <trview.app/Graphics/ILevelTextureStorage.h>
#include <trview.app/Routing/IRoute.h>
#include <trview.app/Camera/ICamera.h>

namespace trview
{
    /// A series of waypoints.
    class Route final : public IRoute
    {
    public:
        /// Create a route.
        explicit Route(const std::unique_ptr<ISelectionRenderer> selection_renderer, const IMesh::Source& mesh_source);

        virtual ~Route() = default;

        Route& operator=(const Route& other);

        /// Add a new waypoint to the end of the route.
        /// @param position The new waypoint.
        /// @param room The room the waypoint is in.
        virtual void add(const DirectX::SimpleMath::Vector3& position, uint32_t room) override;

        /// Add a new waypoint to the end of the route.
        /// @param position The position of the waypoint in the world.
        /// @param room The room that waypoint is in.
        /// @param type The type of the waypoint.
        /// @param type_index The index of the referred to entity or trigger.
        virtual void add(const DirectX::SimpleMath::Vector3& position, uint32_t room, Waypoint::Type type, uint32_t type_index) override;

        /// Remove all of the waypoints from the route.
        virtual void clear() override;

        /// Get the colour of the route.
        virtual Colour colour() const override;

        /// Insert the new waypoint into the route.
        /// @param position The new waypoint.
        /// @param room The room that the waypoint is in.
        /// @param index The index in the route list to put the waypoint.
        virtual void insert(const DirectX::SimpleMath::Vector3& position, uint32_t room, uint32_t index) override;

        /// Insert the new waypoint into the route based on the currently selected waypoint.
        /// @param position The new waypoint.
        /// @param room The room that the waypoint is in.
        /// @return The index of the new waypoint.
        virtual uint32_t insert(const DirectX::SimpleMath::Vector3& position, uint32_t room) override;

        /// Insert a new non-positional waypoint.
        /// @param position The position of the waypoint in the world.
        /// @param room The room that the waypoint is in.
        /// @param type The type of waypoint.
        /// @param index The index in the route list to put the waypoint.
        /// @param type_index The index of the trigger or entity to reference.
        virtual void insert(const DirectX::SimpleMath::Vector3& position, uint32_t room, uint32_t index, Waypoint::Type type, uint32_t type_index) override;

        /// Insert a new non-positional waypoint based on the currently selected waypoint.
        /// @param position The position of the waypoint in the world.
        /// @param room The room that the waypoint is in.
        /// @param type The type of waypoint.
        /// @param type_index The index of the trigger or entity to reference.
        /// @return The index of the new waypoint.
        virtual uint32_t insert(const DirectX::SimpleMath::Vector3& position, uint32_t room, Waypoint::Type type, uint32_t type_index) override;

        /// Pick against the waypoints in the route.
        /// @param position The position of the camera.
        /// @param direction The direction of the ray.
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;

        /// Remove the waypoint at the specified index.
        /// @param index The index of the waypoint to remove.
        virtual void remove(uint32_t index) override;

        /// Render the route.
        /// @param camera The camera to use to render.
        /// @param texture_storage Texture storage for the mesh.
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage) override;

        /// Get the index of the currently selected waypoint.
        virtual uint32_t selected_waypoint() const override;

        /// Set the specified waypoint index to be the selected waypoint.
        /// @param index The index to select.
        virtual void select_waypoint(uint32_t index) override;

        /// Set the colour for the route.
        /// @param colour The colour to use.
        virtual void set_colour(const Colour& colour) override;

        /// Get the waypoint at the specified index.
        /// @param index The index to get.
        virtual const Waypoint& waypoint(uint32_t index) const override;

        /// Get the waypoint at the specified index.
        /// @param index The index to get.
        virtual Waypoint& waypoint(uint32_t index) override;

        /// Get the number of waypoints in the route.
        virtual uint32_t waypoints() const override;
    private:
        uint32_t next_index() const;

        std::vector<Waypoint> _waypoints;
        std::unique_ptr<IMesh> _waypoint_mesh;
        std::unique_ptr<ISelectionRenderer> _selection_renderer;
        uint32_t              _selected_index{ 0u };
        Colour                _colour{ Colour::Green };
    };

    std::unique_ptr<IRoute> import_route(const IRoute::Source& route_source, const std::string& filename);
    void export_route(const IRoute& route, const std::string& filename);
}
