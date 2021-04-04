#pragma once

#include <trlevel/trtypes.h>

#include <d3d11.h>
#include <wrl/client.h>
#include <cstdint>
#include <SimpleMath.h>
#include <trview.app/Camera/ICamera.h>

namespace trview
{
    struct ILevelTextureStorage;
    class Mesh;
    struct ITransparencyBuffer;

    class StaticMesh
    {
    public:
        StaticMesh(const trlevel::tr3_room_staticmesh& static_mesh, const trlevel::tr_staticmesh& level_static_mesh, Mesh* mesh);

        StaticMesh(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Matrix& scale, std::unique_ptr<Mesh> mesh);

        void render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour);

        void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour);
    private:
        float                        _rotation;
        DirectX::SimpleMath::Vector3 _position;
        DirectX::SimpleMath::Vector3 _visibility_min;
        DirectX::SimpleMath::Vector3 _visibility_max;
        DirectX::SimpleMath::Vector3 _collision_min;
        DirectX::SimpleMath::Vector3 _collision_max;
        DirectX::SimpleMath::Matrix  _world;
        Mesh*                        _mesh;
        std::unique_ptr<Mesh> _sprite_mesh;
        DirectX::SimpleMath::Matrix _scale;
    };
}
