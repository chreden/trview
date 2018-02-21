#pragma once

#include <trlevel/trtypes.h>

#include <cstdint>
#include <DirectXMath.h>

namespace trview
{
    class Mesh;

    class StaticMesh
    {
    public:
        StaticMesh(const trlevel::tr_staticmesh& static_mesh, Mesh* mesh);
    private:
        DirectX::XMVECTOR _position;
        DirectX::XMVECTOR _visibility_min;
        DirectX::XMVECTOR _visibility_max;
        DirectX::XMVECTOR _collision_min;
        DirectX::XMVECTOR _collision_max;
        Mesh*             _mesh;
    };
}
