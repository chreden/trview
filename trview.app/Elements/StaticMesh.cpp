#include "StaticMesh.h"
#include <trview.app/Geometry/Matrix.h>
#include <trview.app/Geometry/ITransparencyBuffer.h>

namespace trview
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    namespace
    {
        BoundingBox from_box(const trlevel::tr_bounding_box& box)
        {
            BoundingBox output;
            BoundingBox::CreateFromPoints(output,
                Vector3(box.MinX, box.MinY, box.MinZ),
                Vector3(box.MaxX, box.MaxY, box.MaxZ));
            return output;
        }
    }

    IStaticMesh::~IStaticMesh()
    {
    }

    StaticMesh::StaticMesh(const trlevel::tr3_room_staticmesh& static_mesh, const trlevel::tr_staticmesh& level_static_mesh, const std::shared_ptr<IMesh>& mesh, const std::weak_ptr<IRoom>& room, const std::shared_ptr<IMesh>& bounding_mesh)
        : _mesh(mesh),
        _visibility(from_box(level_static_mesh.VisibilityBox)),
        _collision(from_box(level_static_mesh.CollisionBox)),
        _position(static_mesh.x / trlevel::Scale_X, static_mesh.y / trlevel::Scale_Y, static_mesh.z / trlevel::Scale_Z),
        _rotation(static_mesh.rotation / 16384.0f * DirectX::XM_PIDIV2),
        _bounding_mesh(bounding_mesh),
        _room(room),
        _mesh_texture_id(static_mesh.mesh_id)
    {
        _world = Matrix::CreateRotationY(_rotation) * Matrix::CreateTranslation(_position);
    }

    StaticMesh::StaticMesh(const trlevel::tr_room_sprite& room_sprite, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Matrix& scale, std::shared_ptr<IMesh> mesh, const std::weak_ptr<IRoom>& room)
        : _position(position), _sprite_mesh(mesh), _rotation(0), _scale(scale), _room(room), _mesh_texture_id(room_sprite.texture)
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

    void StaticMesh::render_bounding_box(const ICamera& camera, const ILevelTextureStorage& texture_storage, const DirectX::SimpleMath::Color& colour)
    {
        if (!_sprite_mesh)
        {
            const auto size = (_collision.Extents * 2.0f) / trlevel::Scale;
            const auto adjust = _collision.Center / trlevel::Scale;
            const auto wvp = Matrix::CreateScale(size) * Matrix::CreateTranslation(adjust) * _world * camera.view_projection();
            _bounding_mesh->render(wvp, texture_storage, colour);
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

    Vector3 StaticMesh::position() const
    {
        return _position;
    }

    std::weak_ptr<IRoom> StaticMesh::room() const
    {
        return _room;
    }

    DirectX::BoundingBox StaticMesh::visibility() const
    {
        return _visibility;
    }

    DirectX::BoundingBox StaticMesh::collision() const
    {
        return _collision;
    }

    float StaticMesh::rotation() const
    {
        return _rotation;
    }

    IStaticMesh::Type StaticMesh::type() const
    {
        return _mesh ? Type::Mesh : Type::Sprite;
    }

    uint16_t StaticMesh::id() const
    {
        return _mesh_texture_id;
    }
}
