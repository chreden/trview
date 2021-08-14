#pragma once

#include <trview.app/Graphics/ISelectionRenderer.h>
#include <trview.app/Graphics/ILevelTextureStorage.h>
#include <trview.app/Routing/IRoute.h>
#include <trview.app/Camera/ICamera.h>
#include <trview.common/IFiles.h>

namespace trview
{
    /// A series of waypoints.
    class Route final : public IRoute
    {
    public:
        explicit Route(const std::unique_ptr<ISelectionRenderer> selection_renderer, const IWaypoint::Source& waypoint_source);
        virtual ~Route() = default;
        Route& operator=(const Route& other);
        virtual void add(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room) override;
        virtual void add(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t type_index) override;
        virtual void clear() override;
        virtual Colour colour() const override;
        virtual void insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, uint32_t index) override;
        virtual uint32_t insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room) override;
        virtual void insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, uint32_t index, IWaypoint::Type type, uint32_t type_index) override;
        virtual bool is_unsaved() const override;
        virtual uint32_t insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t type_index) override;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;
        virtual void remove(uint32_t index) override;
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage) override;
        virtual uint32_t selected_waypoint() const override;
        virtual void select_waypoint(uint32_t index) override;
        virtual void set_colour(const Colour& colour) override;
        virtual void set_unsaved(bool value) override;
        virtual const IWaypoint& waypoint(uint32_t index) const override;
        virtual IWaypoint& waypoint(uint32_t index) override;
        virtual uint32_t waypoints() const override;
    private:
        uint32_t next_index() const;

        IWaypoint::Source _waypoint_source;
        std::vector<std::shared_ptr<IWaypoint>> _waypoints;
        std::unique_ptr<ISelectionRenderer> _selection_renderer;
        uint32_t _selected_index{ 0u };
        Colour _colour{ Colour::Green };
        bool _is_unsaved{ false };
    };

    std::shared_ptr<IRoute> import_route(const IRoute::Source& route_source, const std::shared_ptr<IFiles>& files, const std::string& filename);
    void export_route(const IRoute& route, std::shared_ptr<IFiles>& files, const std::string& filename);
}
