#pragma once

#include <cstdint>
#include <SimpleMath.h>
#include <trlevel/trtypes.h>
#include <trview.app/Camera/ICamera.h>
#include <trview.app/Geometry/IMesh.h>
#include <trview.common/Event.h>

#include "../Filters/IFilterable.h"

namespace trview
{
    struct ITransparencyBuffer;
    struct IRoom;
    struct ILevel;

    struct IStaticMesh : public IFilterable
    {
        enum class Type
        {
            Mesh,
            Sprite
        };

        using PositionSource = std::function<std::shared_ptr<IStaticMesh>(const trlevel::tr_room_sprite&, const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Matrix&, std::shared_ptr<IMesh>, const std::shared_ptr<IRoom>&, const std::weak_ptr<ILevel>&)>;
        using MeshSource = std::function<std::shared_ptr<IStaticMesh>(const trlevel::tr3_room_staticmesh&, const trlevel::tr_staticmesh&, const std::shared_ptr<IMesh>&, const std::shared_ptr<IRoom>&, const std::weak_ptr<ILevel>&)>;

        virtual ~IStaticMesh() = 0;
        virtual void render(const ICamera& camera, const DirectX::SimpleMath::Color& colour) = 0;
        virtual void render_bounding_box(const ICamera& camera, const DirectX::SimpleMath::Color& colour) = 0;
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) = 0;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;

        virtual DirectX::SimpleMath::Vector3 position() const = 0;
        virtual std::weak_ptr<IRoom> room() const = 0;
        virtual float rotation() const = 0;
        virtual DirectX::BoundingBox visibility() const = 0;
        virtual DirectX::BoundingBox collision() const = 0;
        virtual Type type() const = 0;
        virtual uint16_t id() const = 0;
        virtual void set_number(uint32_t number) = 0;
        virtual uint32_t number() const = 0;
        virtual uint16_t flags() const = 0;
        virtual bool breakable() const = 0;
        virtual bool visible() const = 0;
        virtual void set_visible(bool value) = 0;
        virtual bool has_collision() const = 0;
        virtual std::weak_ptr<ILevel> level() const = 0;

        Event<> on_changed;
    };

    constexpr std::string to_string(IStaticMesh::Type type) noexcept;

    uint32_t static_mesh_room(const std::shared_ptr<IStaticMesh>& static_mesh);
    uint32_t static_mesh_room(const IStaticMesh& static_mesh);
}

#include "IStaticMesh.inl"