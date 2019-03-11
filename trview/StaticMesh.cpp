#include "stdafx.h"
#include "StaticMesh.h"
#include <trview.app/Geometry/Mesh.h>
#include <trview.app/TransparencyBuffer.h>

namespace trview
{
    StaticMesh::StaticMesh(const trlevel::tr3_room_staticmesh& static_mesh, const trlevel::tr_staticmesh& level_static_mesh, Mesh* mesh)
        : _mesh(mesh),
        _visibility_min(level_static_mesh.VisibilityBox.MinX, level_static_mesh.VisibilityBox.MinY, level_static_mesh.VisibilityBox.MinZ),
        _visibility_max(level_static_mesh.VisibilityBox.MaxX, level_static_mesh.VisibilityBox.MaxY, level_static_mesh.VisibilityBox.MaxZ),
        _collision_min(level_static_mesh.CollisionBox.MinX, level_static_mesh.CollisionBox.MinY, level_static_mesh.CollisionBox.MinZ),
        _collision_max(level_static_mesh.CollisionBox.MaxX, level_static_mesh.CollisionBox.MaxY, level_static_mesh.CollisionBox.MaxZ),
        _position(static_mesh.x / trlevel::Scale_X, static_mesh.y / trlevel::Scale_Y, static_mesh.z / trlevel::Scale_Z),
        _rotation(static_mesh.rotation / 16384.0f * DirectX::XM_PIDIV2)
    {
        using namespace DirectX::SimpleMath;
        _world = Matrix::CreateRotationY(_rotation) * Matrix::CreateTranslation(_position);
    }

    void StaticMesh::render(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const DirectX::SimpleMath::Matrix& view_projection, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour)
    {
        _mesh->render(context, _world * view_projection, texture_storage, colour);
    }

    void StaticMesh::get_transparent_triangles(TransparencyBuffer& transparency, const DirectX::SimpleMath::Color& colour)
    {
        for (const auto& triangle : _mesh->transparent_triangles())
        {
            transparency.add(triangle.transform(_world, colour));
        }
    }
}
