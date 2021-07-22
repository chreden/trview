#pragma once

#include <trview.app/Geometry/IMesh.h>
#include "IWaypoint.h"

namespace trview
{
    /// A waypoint is an entry in a route.
    class Waypoint final : public IWaypoint
    {
    public:
        /// Create a new waypoint of position type.
        /// @param mesh The waypoint mesh.
        /// @param position The position of the waypoint in the world.
        /// @param room The room that the waypoint is in.
        explicit Waypoint(IMesh* mesh, const DirectX::SimpleMath::Vector3& position, uint32_t room);

        /// Create a new waypoint.
        /// @param mesh The waypoint mesh.
        /// @param position The position of the waypoint in the world.
        /// @param room The room that waypoint is in.
        /// @param type The type of waypoint.
        /// @param index The index of the entity or trigger being referenced if this is a non-position type.
        /// @param route_colour The colour of the route.
        explicit Waypoint(IMesh* mesh, const DirectX::SimpleMath::Vector3& position, uint32_t room, Type type, uint32_t index, const Colour& route_colour);

        /// Destructor for waypoint.
        virtual ~Waypoint() = default;

        /// Render the waypoint in the 3D view.
        /// @param camera The current camera being used for rendering.
        /// @param texture_storage The current texture storage instance.
        /// @param colour The colour to render this object.
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) override;

        /// Get the transparent triangles that are contained in this object.
        /// @param transparency The transparency buffer to add triangles to.
        /// @param camera The current camera being used for rendering.
        /// @param colour The colour to render the triangles.
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        virtual DirectX::SimpleMath::Vector3 position() const override;
        virtual Type type() const override;
        virtual bool has_save() const override;
        virtual uint32_t index() const override;
        virtual uint32_t room() const override;
        virtual std::wstring notes() const override;
        virtual std::vector<uint8_t> save_file() const override;
        virtual void set_notes(const std::wstring& notes) override;
        virtual void set_route_colour(const Colour& colour) override;
        virtual void set_save_file(const std::vector<uint8_t>& data) override;

        virtual bool visible() const override;
        virtual void set_visible(bool value) override;
    private:
        std::wstring                 _notes;
        std::vector<uint8_t>         _save_data;
        DirectX::SimpleMath::Vector3 _position;
        IMesh*                       _mesh;
        Type                         _type;
        uint32_t                     _index;
        uint32_t                     _room;
        Colour                       _route_colour;
        bool                         _visible{ true };
    };    
}
