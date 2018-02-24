#include "stdafx.h"
#include "StaticMesh.h"
#include "Mesh.h"

namespace trview
{
    StaticMesh::StaticMesh(const trlevel::tr_staticmesh& static_mesh, Mesh* mesh)
        : _mesh(mesh),
        _visibility_min(DirectX::XMVectorSet(static_mesh.VisibilityBox.MinX, static_mesh.VisibilityBox.MinY, static_mesh.VisibilityBox.MinZ, 0)),
        _visibility_max(DirectX::XMVectorSet(static_mesh.VisibilityBox.MaxX, static_mesh.VisibilityBox.MaxY, static_mesh.VisibilityBox.MaxZ, 0)),
        _collision_min(DirectX::XMVectorSet(static_mesh.CollisionBox.MinX, static_mesh.CollisionBox.MinY, static_mesh.CollisionBox.MinZ, 0)),
        _collision_max(DirectX::XMVectorSet(static_mesh.CollisionBox.MaxX, static_mesh.CollisionBox.MaxY, static_mesh.CollisionBox.MaxZ, 0))
    {
        using namespace DirectX;
        _position = XMVectorAdd(_visibility_min, XMVectorScale(XMVectorSubtract(_visibility_max, _visibility_min), 0.5f));
    }

    void StaticMesh::render(CComPtr<ID3D11DeviceContext> context, const DirectX::XMMATRIX& view_projection)
    {
        using namespace DirectX;
        auto world_view_projection = XMMatrixTranslationFromVector(_position);
        _mesh->render(context, world_view_projection);
    }
}
