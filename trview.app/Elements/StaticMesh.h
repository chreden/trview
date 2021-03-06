#pragma once

#include <trview.app/Elements/IStaticMesh.h>

namespace trview
{
    class StaticMesh final : public IStaticMesh
    {
    public:
        StaticMesh(const trlevel::tr3_room_staticmesh& static_mesh, const trlevel::tr_staticmesh& level_static_mesh, const std::shared_ptr<IMesh>& mesh);
        StaticMesh(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Matrix& scale, std::shared_ptr<IMesh> mesh);
        virtual ~StaticMesh() = default;
        virtual void render(const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour) override;
        virtual void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const override;
    private:
        float                        _rotation;
        DirectX::SimpleMath::Vector3 _position;
        DirectX::SimpleMath::Vector3 _visibility_min;
        DirectX::SimpleMath::Vector3 _visibility_max;
        DirectX::SimpleMath::Vector3 _collision_min;
        DirectX::SimpleMath::Vector3 _collision_max;
        DirectX::SimpleMath::Matrix  _world;
        std::shared_ptr<IMesh> _mesh;
        std::shared_ptr<IMesh> _sprite_mesh;
        DirectX::SimpleMath::Matrix _scale;
    };
}
