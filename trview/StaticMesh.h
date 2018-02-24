#pragma once

#include <trlevel/trtypes.h>

#include <d3d11.h>
#include <atlbase.h>
#include <cstdint>
#include <DirectXMath.h>

namespace trview
{
    class Mesh;

    class StaticMesh
    {
    public:
        StaticMesh(const trlevel::tr_staticmesh& static_mesh, Mesh* mesh);

        void render(CComPtr<ID3D11DeviceContext> context, const DirectX::XMMATRIX& view_projection);
    private:
        DirectX::XMVECTOR _position;
        DirectX::XMVECTOR _visibility_min;
        DirectX::XMVECTOR _visibility_max;
        DirectX::XMVECTOR _collision_min;
        DirectX::XMVECTOR _collision_max;
        Mesh*             _mesh;
    };
}
