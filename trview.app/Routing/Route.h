#pragma once

#include <trview.app/Graphics/ISelectionRenderer.h>
#include <trview.app/Graphics/ILevelTextureStorage.h>
#include <trview.app/Routing/IRoute.h>
#include <trview.app/Camera/ICamera.h>
#include <trview.common/IFiles.h>
#include "../Settings/UserSettings.h"

namespace trview
{
    struct ILevel;

    /// A series of waypoints.
    class Route final : public IRoute, public std::enable_shared_from_this<Route>
    {
    public:
        explicit Route(const std::unique_ptr<ISelectionRenderer> selection_renderer, const IWaypoint::Source& waypoint_source, const UserSettings& settings);
        virtual ~Route() = default;
        Route& operator=(const Route& other);
        std::shared_ptr<IWaypoint> add(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room) override;
        std::shared_ptr<IWaypoint> add(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t type_index) override;
        std::shared_ptr<IWaypoint> add(const std::shared_ptr<IWaypoint>& waypoint) override;
        virtual void clear() override;
        virtual Colour colour() const override;
        std::optional<std::string> filename() const;
        virtual void insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, uint32_t index) override;
        virtual uint32_t insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room) override;
        virtual void insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, uint32_t index, IWaypoint::Type type, uint32_t type_index) override;
        virtual bool is_unsaved() const override;
        virtual uint32_t insert(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t type_index) override;
        std::weak_ptr<ILevel> level() const override;
        virtual void move(int32_t left, int32_t right) override;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;
        void reload(const std::shared_ptr<IFiles>& files, const UserSettings& settings) override;
        virtual void remove(uint32_t index) override;
        void remove(const std::shared_ptr<IWaypoint>& waypoint) override;
        void render(const ICamera& camera, bool show_selection) override;
        void save(const std::shared_ptr<IFiles>& files, const UserSettings& settings) override;
        void save_as(const std::shared_ptr<IFiles>& files, const std::string& filename, const UserSettings& settings) override;
        virtual uint32_t selected_waypoint() const override;
        void select_waypoint(const std::weak_ptr<IWaypoint>& waypoint) override;
        virtual void set_colour(const Colour& colour) override;
        void set_filename(const std::string& filename) override;
        void set_level(const std::weak_ptr<ILevel>& level) override;
        virtual void set_waypoint_colour(const Colour& colour) override;
        virtual void set_unsaved(bool value) override;
        void set_show_route_line(bool show) override;
        bool show_route_line() const override;
        virtual Colour waypoint_colour() const override;
        virtual std::weak_ptr<IWaypoint> waypoint(uint32_t index) const override;
        virtual uint32_t waypoints() const override;

        void import(const std::vector<uint8_t>& data);
    private:
        uint32_t next_index() const;
        void bind_waypoint_targets();
        void bind_waypoint(IWaypoint& waypoint);
        void unbind_waypoint(IWaypoint& waypoint);

        IWaypoint::Source _waypoint_source;
        std::vector<std::shared_ptr<IWaypoint>> _waypoints;
        std::unique_ptr<ISelectionRenderer> _selection_renderer;
        uint32_t _selected_index{ 0u };
        Colour _colour{ Colour::Green };
        Colour _waypoint_colour{ Colour::White };
        bool _is_unsaved{ false };
        std::weak_ptr<ILevel> _level;
        std::optional<std::string> _filename;
        bool _show_route_line{ true };
    };
}
