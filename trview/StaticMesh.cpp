#include "stdafx.h"
#include "StaticMesh.h"
#include "Mesh.h"

namespace trview
{
    StaticMesh::StaticMesh(const trlevel::tr3_room_staticmesh& static_mesh, const trlevel::tr_staticmesh& level_static_mesh, Mesh* mesh)
        : _mesh(mesh),
        _visibility_min(DirectX::XMVectorSet(level_static_mesh.VisibilityBox.MinX, level_static_mesh.VisibilityBox.MinY, level_static_mesh.VisibilityBox.MinZ, 0)),
        _visibility_max(DirectX::XMVectorSet(level_static_mesh.VisibilityBox.MaxX, level_static_mesh.VisibilityBox.MaxY, level_static_mesh.VisibilityBox.MaxZ, 0)),
        _collision_min(DirectX::XMVectorSet(level_static_mesh.CollisionBox.MinX, level_static_mesh.CollisionBox.MinY, level_static_mesh.CollisionBox.MinZ, 0)),
        _collision_max(DirectX::XMVectorSet(level_static_mesh.CollisionBox.MaxX, level_static_mesh.CollisionBox.MaxY, level_static_mesh.CollisionBox.MaxZ, 0))
    {
        using namespace DirectX;
        _rotation = XMConvertToRadians(static_mesh.rotation / 16384.0f * 90.0f);
        _position = XMVectorSet(static_mesh.x / 1024.0f, static_mesh.y / -1024.0f, static_mesh.z / 1024.0f, 1);
    }

    void StaticMesh::render(CComPtr<ID3D11DeviceContext> context, const DirectX::XMMATRIX& view_projection, const ILevelTextureStorage& texture_storage, const DirectX::XMFLOAT4& colour)
    {
        using namespace DirectX;
        auto world_view_projection = 
            XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationY(_rotation),XMMatrixTranslationFromVector(_position)), view_projection);
        _mesh->render(context, world_view_projection, texture_storage, colour);
    }
}
