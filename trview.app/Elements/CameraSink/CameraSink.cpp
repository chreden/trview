#include "CameraSink.h"
#include "../ILevel.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace trview
{
    CameraSink::CameraSink(const std::shared_ptr<IMesh>& mesh, const std::shared_ptr<ITextureStorage>& texture_storage, 
        uint32_t number, const trlevel::tr_camera& camera, Type type, const std::vector<std::weak_ptr<IRoom>>& inferred_rooms, const std::vector<std::weak_ptr<ITrigger>>& triggers,
        const std::weak_ptr<ILevel>& level)
        : _mesh(mesh), _number(number), _position(camera.position()), _strength(camera.Room), _flag(camera.Flag), _inferred_rooms(inferred_rooms), _type(type), _triggers(triggers),
        _level(level)
    {
        if (const auto new_level = _level.lock())
        {
            _room = new_level->room(camera.Room);
        }
        _camera_texture = texture_storage->lookup("camera_texture");
        _sink_texture = texture_storage->lookup("sink_texture");
    }

    ICameraSink::~ICameraSink()
    {
    }

    DirectX::BoundingBox CameraSink::bounding_box() const
    {
        return BoundingBox(_position, Vector3(0.125f, 0.125f, 0.125f));
    }

    uint16_t CameraSink::box_index() const
    {
        return _flag;
    }

    uint16_t CameraSink::flag() const
    {
        return _flag;
    }

    void CameraSink::get_transparent_triangles(ITransparencyBuffer&, const ICamera&, const Color&)
    {
    }

    std::weak_ptr<ILevel> CameraSink::level() const
    {
        return _level;
    }

    std::vector<std::weak_ptr<IRoom>> CameraSink::inferred_rooms() const
    {
        return _inferred_rooms;
    }

    uint32_t CameraSink::number() const
    {
        return _number;
    }

    bool CameraSink::persistent() const
    {
        return (_flag & 0x1) == 1;
    }

    PickResult CameraSink::pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const
    {
        PickResult result{};
        BoundingBox cube(_position, Vector3(0.125f, 0.125f, 0.125f));

        float distance = 0;
        if (cube.Intersects(position, direction, distance))
        {
            result.distance = distance;
            result.hit = true;
            result.camera_sink = std::const_pointer_cast<ICameraSink>(shared_from_this());
            result.position = position + direction * distance;
            result.type = PickResult::Type::CameraSink;
        }

        return result;
    }

    Vector3 CameraSink::position() const
    {
        return _position;
    }

    void CameraSink::render(const ICamera& camera, const Color&)
    {
        if (!_visible)
        {
            return;
        }

        auto world = Matrix::CreateScale(0.25f) * Matrix::CreateTranslation(_position);
        auto wvp = world * camera.view_projection();
        auto light_direction = Vector3::TransformNormal(camera.position() - _position, world.Invert());
        light_direction.Normalize();
        _mesh->render(wvp, type() == Type::Camera ? _camera_texture : _sink_texture, Colour::White, 1.0f, light_direction);
    }

    std::weak_ptr<IRoom> CameraSink::room() const
    {
        return _room;
    }

    void CameraSink::set_type(Type type)
    {
        _type = type;
    }

    void CameraSink::set_visible(bool value)
    {
        _visible = value;
        on_changed();
    }

    uint16_t CameraSink::strength() const
    {
        return _strength;
    }

    CameraSink::Type CameraSink::type() const
    {
        return _type;
    }

    bool CameraSink::visible() const
    {
        return _visible;
    }

    std::vector<std::weak_ptr<ITrigger>> CameraSink::triggers() const
    {
        return _triggers;
    }

    int32_t CameraSink::filterable_index() const
    {
        return static_cast<int32_t>(_number);
    }

    std::weak_ptr<IRoom> actual_room(const ICameraSink& camera_sink)
    {
        if (camera_sink.type() == ICameraSink::Type::Camera)
        {
            return camera_sink.room();
        }
        const auto inferred = camera_sink.inferred_rooms();
        return inferred.empty() ? std::weak_ptr<IRoom>{} : inferred.front();
    }

    std::string to_string(ICameraSink::Type type)
    {
        switch (type)
        {
        case ICameraSink::Type::Camera:
            return "Camera";
        case ICameraSink::Type::Sink:
            return "Sink";
        }
        return "?";
    }
}
