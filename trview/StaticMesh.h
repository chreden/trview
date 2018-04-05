#pragma once

#include <trlevel/trtypes.h>

#include <d3d11.h>
#include <atlbase.h>
#include <cstdint>
#include <DirectXMath.h>
#include <external/DirectXTK/Inc/SimpleMath.h>

namespace trview
{
    struct ILevelTextureStorage;
    class Mesh;

    class StaticMesh
    {
    public:
        StaticMesh(const trlevel::tr3_room_staticmesh& static_mesh, const trlevel::tr_staticmesh& level_static_mesh, Mesh* mesh);

        void render(CComPtr<ID3D11DeviceContext> context, const DirectX::SimpleMath::Matrix& view_projection, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour);
    private:
        float                        _rotation;
        DirectX::SimpleMath::Vector3 _position;
        DirectX::SimpleMath::Vector3 _visibility_min;
        DirectX::SimpleMath::Vector3 _visibility_max;
        DirectX::SimpleMath::Vector3 _collision_min;
        DirectX::SimpleMath::Vector3 _collision_max;
        DirectX::SimpleMath::Matrix  _world;
        Mesh*                        _mesh;
    };
}
