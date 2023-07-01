#pragma once

#include "IRoute.h"
#include "IRandomizerRoute.h"

#include <map>
#include <vector>

namespace trview
{
    /// <summary>
    /// Version of route that has a set of unbound waypoints that are eventually
    /// converted to bound ones. It then exposes the route through the interface.
    /// </summary>
    class RandomizerRoute final : public IRandomizerRoute
    {
    public:
        explicit RandomizerRoute(const std::shared_ptr<IRoute>& inner_route, const IWaypoint::Source& waypoint_source);
        virtual ~RandomizerRoute() = default;
        std::shared_ptr<IWaypoint> add(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room) override;
        std::shared_ptr<IWaypoint> add(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t type_index) override;
        std::shared_ptr<IWaypoint> add(const std::shared_ptr<IWaypoint>& waypoint) override;
        std::shared_ptr<IWaypoint> add(const std::string& level_name, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room_number) override;
        void clear() override;
        Colour colour() const override;
        void insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, uint32_t index) override;
        uint32_t insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room) override;
        void insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, uint32_t index, IWaypoint::Type type, uint32_t type_index) override;
        uint32_t insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t type_index) override;
        bool is_unsaved() const override;
        std::weak_ptr<ILevel> level() const override;
        void move(int32_t from, int32_t to) override;
        PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;
        void remove(uint32_t index) override;
        void remove(const std::shared_ptr<IWaypoint>& waypoint) override;
        void render(const ICamera& camera, const ILevelTextureStorage& texture_storage, bool show_selection) override;
        uint32_t selected_waypoint() const override;
        void select_waypoint(uint32_t index) override;
        void set_colour(const Colour& colour) override;
        void set_level(const std::weak_ptr<ILevel>& level) override;
        void set_randomizer_enabled(bool enabled) override;
        void set_waypoint_colour(const Colour& colour) override;
        void set_unsaved(bool value) override;
        Colour waypoint_colour() const override;
        std::weak_ptr<IWaypoint> waypoint(uint32_t index) const override;
        uint32_t waypoints() const override;
    private:
        std::shared_ptr<IRoute> _route;
        std::map<std::string, std::vector<std::shared_ptr<IWaypoint>>> _waypoints;
        IWaypoint::Source _waypoint_source;
    };
}
