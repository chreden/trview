#pragma once

#include <trview.app/Elements/IStaticMesh.h>

namespace trview
{
    class StaticMesh final : public IStaticMesh
    {
    public:
        StaticMesh(const trlevel::tr3_room_staticmesh& static_mesh, const trlevel::tr_staticmesh& level_static_mesh, const std::shared_ptr<IMesh>& mesh, const std::weak_ptr<IRoom>& room, const std::shared_ptr<IMesh>& bounding_mesh);
        StaticMesh(const trlevel::tr_room_sprite& room_sprite, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Matrix& scale, std::shared_ptr<IMesh> mesh, const std::weak_ptr<IRoom>& room);
        virtual ~StaticMesh() = default;
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) override;
        virtual void render_bounding_box(const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) override;
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;

        DirectX::SimpleMath::Vector3 position() const override;
        std::weak_ptr<IRoom> room() const override;
        DirectX::BoundingBox visibility() const override;
        DirectX::BoundingBox collision() const override;
        float rotation() const override;
        Type type() const override;
        uint16_t id() const override;
    private:
        float _rotation;
        DirectX::SimpleMath::Vector3 _position;
        DirectX::BoundingBox _visibility;
        DirectX::BoundingBox _collision;
        DirectX::SimpleMath::Matrix  _world;
        std::shared_ptr<IMesh> _mesh;
        std::shared_ptr<IMesh> _sprite_mesh;
        std::shared_ptr<IMesh> _bounding_mesh;
        DirectX::SimpleMath::Matrix _scale;
        std::weak_ptr<IRoom> _room;
        uint16_t _mesh_texture_id{ 0u };
    };
}
