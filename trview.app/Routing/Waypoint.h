#pragma once

#include <trview.app/Geometry/IMesh.h>
#include <variant>
#include "IWaypoint.h"

namespace trview
{
    /// <summary>
    /// A waypoint is an entry in a route.
    /// </summary>
    class Waypoint final : public IWaypoint
    {
    public:
        /// <summary>
        /// Create a new waypoint.
        /// </summary>
        /// <param name="mesh">The waypoint mesh.</param>
        /// <param name="position">The position of the waypoint in the world.</param>
        /// <param name="normal">The normal to which the waypoint is aligned.</param>
        /// <param name="room">The room that waypoint is in.</param>
        /// <param name="type">The type of waypoint.</param>
        /// <param name="index">The index of the entity or trigger being referenced if this is a non-position type.</param>
        /// <param name="route_colour">The colour of the route.</param>
        explicit Waypoint(std::shared_ptr<IMesh> mesh, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, Type type, uint32_t index, const Colour& route_colour, const Colour& stick_colour);
        virtual ~Waypoint() = default;
        void render(const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        void render_join(const IWaypoint& next_waypoint, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        virtual DirectX::BoundingBox bounding_box() const override;
        virtual DirectX::SimpleMath::Vector3 position() const override;
        virtual Type type() const override;
        virtual bool has_save() const override;
        virtual uint32_t index() const override;
        std::weak_ptr<IItem> item() const override;
        virtual std::string notes() const override;
        virtual uint32_t room() const override;
        std::weak_ptr<IRoute> route() const override;
        virtual std::vector<uint8_t> save_file() const override;
        void set_item(const std::weak_ptr<IItem>& item) override;
        virtual void set_notes(const std::string& notes) override;
        void set_route(const std::weak_ptr<IRoute>& route) override;
        virtual void set_route_colour(const Colour& colour) override;
        virtual void set_save_file(const std::vector<uint8_t>& data) override;
        void set_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        virtual void set_waypoint_colour(const Colour& colour) override;
        virtual DirectX::SimpleMath::Vector3 blob_position() const override;
        virtual bool visible() const override;
        virtual void set_visible(bool value) override;
        virtual DirectX::SimpleMath::Vector3 normal() const override;
        virtual WaypointRandomizerSettings randomizer_settings() const override;
        virtual void set_randomizer_settings(const WaypointRandomizerSettings& settings) override;
        virtual void set_position(const DirectX::SimpleMath::Vector3& position) override;
        std::weak_ptr<ITrigger> trigger() const override;
        Colour waypoint_colour() const override;
        Colour route_colour() const override;
        void set_normal(const DirectX::SimpleMath::Vector3& normal) override;
        void set_room_number(uint32_t room) override;
        DirectX::SimpleMath::Vector2 screen_position() const override;
    private:
        DirectX::SimpleMath::Matrix calculate_waypoint_rotation() const;
        void set_properties(Type type, uint32_t index, uint32_t room, const DirectX::SimpleMath::Vector3& position);

        std::string _notes;
        std::vector<uint8_t> _save_data;
        DirectX::SimpleMath::Vector3 _position;
        DirectX::SimpleMath::Vector3 _normal;
        std::shared_ptr<IMesh> _mesh;
        Type _type;
        uint32_t _index;
        uint32_t _room;
        Colour _route_colour;
        Colour _waypoint_colour;
        bool _visible{ true };
        WaypointRandomizerSettings _randomizer_settings;
        std::weak_ptr<IRoute> _route;

        mutable std::weak_ptr<IItem> _item;
        mutable std::weak_ptr<ITrigger> _trigger;

        DirectX::SimpleMath::Vector2 _screen_position{ -1, -1 };
    };
}
