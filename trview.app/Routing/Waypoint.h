#pragma once

#include <trview.app/Geometry/IMesh.h>
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
        explicit Waypoint(std::shared_ptr<IMesh> mesh, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, uint32_t room, Type type, uint32_t index, const Colour& route_colour);
        virtual ~Waypoint() = default;
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) override;
        virtual void render_join(const IWaypoint& next_waypoint, const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) override;
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        virtual DirectX::BoundingBox bounding_box() const override;
        virtual std::string difficulty() const override;
        virtual DirectX::SimpleMath::Vector3 position() const override;
        virtual Type type() const override;
        virtual bool has_save() const override;
        virtual uint32_t index() const override;
        virtual bool is_in_room_space() const override;
        virtual bool is_item() const override;
        virtual std::wstring notes() const override;
        virtual uint32_t room() const override;
        virtual bool requires_glitch() const override;
        virtual std::vector<uint8_t> save_file() const override;
        virtual void set_difficulty(const std::string& value) override;
        virtual void set_is_in_room_space(bool value) override;
        virtual void set_is_item(bool value) override;
        virtual void set_notes(const std::wstring& notes) override;
        virtual void set_requires_glitch(bool value) override;
        virtual void set_route_colour(const Colour& colour) override;
        virtual void set_save_file(const std::vector<uint8_t>& data) override;
        virtual void set_vehicle_required(bool value) override;
        virtual DirectX::SimpleMath::Vector3 blob_position() const override;
        virtual bool vehicle_required() const override;
        virtual bool visible() const override;
        virtual void set_visible(bool value) override;
        virtual DirectX::SimpleMath::Vector3 normal() const override;
    private:
        DirectX::SimpleMath::Matrix calculate_waypoint_rotation() const;

        std::wstring _notes;
        std::vector<uint8_t> _save_data;
        DirectX::SimpleMath::Vector3 _position;
        DirectX::SimpleMath::Vector3 _normal;
        std::shared_ptr<IMesh> _mesh;
        Type _type;
        uint32_t _index;
        uint32_t _room;
        Colour _route_colour;
        bool _visible{ true };
        bool _requires_glitch{ false };
        bool _vehicle_required{ false };
        bool _is_in_room_space{ true };
        bool _is_item{ false };
        std::string _difficulty{ "Easy" };
    };    
}
