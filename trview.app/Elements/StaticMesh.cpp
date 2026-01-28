#include "StaticMesh.h"
#include <trview.app/Geometry/Matrix.h>
#include <trview.app/Geometry/ITransparencyBuffer.h>
#include "IRoom.h"

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

    StaticMesh::StaticMesh(const trlevel::tr3_room_staticmesh& static_mesh, const trlevel::tr_staticmesh& level_static_mesh, const std::shared_ptr<IMesh>& mesh, const std::weak_ptr<IRoom>& room, const std::weak_ptr<ILevel>& level, const std::shared_ptr<IMesh>& bounding_mesh)
        : _mesh(mesh),
        _visibility(from_box(level_static_mesh.VisibilityBox)),
        _collision(from_box(level_static_mesh.CollisionBox)),
        _position(static_mesh.x / trlevel::Scale_X, static_mesh.y / trlevel::Scale_Y, static_mesh.z / trlevel::Scale_Z),
        _rotation(static_mesh.rotation / 16384.0f * DirectX::XM_PIDIV2),
        _bounding_mesh(bounding_mesh),
        _room(room),
        _mesh_texture_id(static_mesh.mesh_id),
        _flags(level_static_mesh.Flags),
        _level(level)
    {
        _world = Matrix::CreateRotationY(_rotation) * Matrix::CreateTranslation(_position);
    }

    StaticMesh::StaticMesh(const trlevel::tr_room_sprite& room_sprite, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Matrix& scale, std::shared_ptr<IMesh> mesh, const std::weak_ptr<IRoom>& room, const std::weak_ptr<ILevel>& level)
        : _position(position), _mesh(mesh), _rotation(0), _scale(scale), _room(room), _mesh_texture_id(room_sprite.texture), _type(Type::Sprite), _level(level)
    {
        using namespace DirectX::SimpleMath;
        _world = Matrix::CreateRotationY(_rotation) * Matrix::CreateTranslation(_position);
    }

    void StaticMesh::render(const ICamera& camera, const DirectX::SimpleMath::Color& colour)
    {
        if (!_mesh)
        {
            return;
        }

        if (_type == Type::Sprite)
        {
            _world = create_billboard(_position, Vector3(), _scale, camera);
        }
        _mesh->render(_world * camera.view_projection(), colour);
    }

    void StaticMesh::render_bounding_box(const ICamera& camera, const DirectX::SimpleMath::Color& colour)
    {
        if (_type == Type::Mesh)
        {
            const auto size = (_collision.Extents * 2.0f) / trlevel::Scale;
            const auto adjust = _collision.Center / trlevel::Scale;
            const auto wvp = Matrix::CreateScale(size) * Matrix::CreateTranslation(adjust) * _world * camera.view_projection();
            _bounding_mesh->render(wvp, colour);
        }
    }

    void StaticMesh::get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour)
    {
        if (!_mesh)
        {
            return;
        }

        if (_type == Type::Sprite)
        {
            _world = create_billboard(_position, Vector3(), _scale, camera);
        }

        for (const auto& triangle : _mesh->transparent_triangles())
        {
            transparency.add(triangle.transform(_world, colour, true));
        }
    }

    PickResult StaticMesh::pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const
    {
        if (!_mesh)
        {
            return {};
        }

        const auto transform = _world.Invert();
        auto normal_direction = Vector3::TransformNormal(direction, transform);
        normal_direction.Normalize();
        PickResult result = _mesh->pick(Vector3::Transform(position, transform), normal_direction);
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
        return _type;
    }

    uint16_t StaticMesh::id() const
    {
        return _mesh_texture_id;
    }

    void StaticMesh::set_number(uint32_t number)
    {
        _number = number;
    }
    
    uint32_t StaticMesh::number() const
    {
        return _number;
    }

    uint16_t StaticMesh::flags() const
    {
        return _flags;
    }

    bool StaticMesh::breakable() const
    {
        return _mesh_texture_id >= 50 && _mesh_texture_id <= 69;
    }

    bool StaticMesh::visible() const
    {
        return _visible;
    }

    void StaticMesh::set_visible(bool value)
    {
        if (value != _visible)
        {
            _visible = value;
            on_changed();
        }
    }

    bool StaticMesh::has_collision() const
    {
        return _type == Type::Mesh && (_flags & 0x1) == 0;
    }

    std::weak_ptr<ILevel> StaticMesh::level() const
    {
        return _level;
    }

    int32_t StaticMesh::filterable_index() const
    {
        return static_cast<int32_t>(_number);
    }

    uint32_t static_mesh_room(const std::shared_ptr<IStaticMesh>& static_mesh)
    {
        if (!static_mesh)
        {
            return 0u;
        }
        return static_mesh_room(*static_mesh);
    }

    uint32_t static_mesh_room(const IStaticMesh& static_mesh)
    {
        if (auto room = static_mesh.room().lock())
        {
            return room->number();
        }
        return 0u;
    }
}
