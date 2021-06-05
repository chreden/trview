#include "StaticMesh.h"
#include <trview.app/Geometry/Matrix.h>
#include <trview.app/Geometry/ITransparencyBuffer.h>

namespace trview
{
    using namespace DirectX::SimpleMath;

    StaticMesh::StaticMesh(const trlevel::tr3_room_staticmesh& static_mesh, const trlevel::tr_staticmesh& level_static_mesh, const std::shared_ptr<IMesh>& mesh)
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

    StaticMesh::StaticMesh(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Matrix& scale, std::shared_ptr<IMesh> mesh)
        : _position(position), _sprite_mesh(mesh), _rotation(0), _scale(scale)
    {
        using namespace DirectX::SimpleMath;
        _world = Matrix::CreateRotationY(_rotation) * Matrix::CreateTranslation(_position);
    }

    void StaticMesh::render(const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour)
    {
        if (_sprite_mesh)
        {
            auto wvp = create_billboard(_position, Vector3(0, -0.5f, 0), _scale, camera) * camera.view_projection();
            _sprite_mesh->render(wvp, texture_storage, colour);
        }
        else
        {
            _mesh->render(_world * camera.view_projection(), texture_storage, colour);
        }
    }

    void StaticMesh::get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour)
    {
        if (_sprite_mesh)
        {
            auto world = create_billboard(_position, Vector3(0, -0.5f, 0), _scale, camera);
            for (const auto& triangle : _sprite_mesh->transparent_triangles())
            {
                transparency.add(triangle.transform(world, colour));
            }
        }
        else
        {
            for (const auto& triangle : _mesh->transparent_triangles())
            {
                transparency.add(triangle.transform(_world, colour));
            }
        }
    }

    PickResult StaticMesh::pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const
    {
        if (_sprite_mesh)
        {
            return {};
        }

        PickResult result = _mesh->pick(Vector3::Transform(position, _world.Invert()), direction);
        result.position = Vector3::Transform(result.position, _world);
        return result;
    }
}
