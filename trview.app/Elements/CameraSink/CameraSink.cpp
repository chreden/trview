#include "CameraSink.h"
#include <ranges>

using namespace DirectX::SimpleMath;

namespace trview
{
    CameraSink::CameraSink(uint32_t number, const trlevel::tr_camera& camera, const std::vector<uint16_t>& inferred_rooms)
        : _number(number), _position(camera.position()), _room(camera.Room), _flag(camera.Flag), _inferred_rooms(inferred_rooms)
    {
        if (!std::ranges::contains(_inferred_rooms, _room))
        {
            _type = Type::Sink;
        }
    }

    ICameraSink::~ICameraSink()
    {
    }

    uint16_t CameraSink::flag() const
    {
        return _flag;
    }

    void CameraSink::get_transparent_triangles(ITransparencyBuffer&, const ICamera&, const Color&)
    {
    }

    std::vector<uint16_t> CameraSink::inferred_rooms() const
    {
        return _inferred_rooms;
    }

    uint32_t CameraSink::number() const
    {
        return _number;
    }

    Vector3 CameraSink::position() const
    {
        return _position;
    }

    void CameraSink::render(const ICamera&, const ILevelTextureStorage&, const Color&)
    {
        if (!_visible)
        {
            return;
        }

        // auto world = Matrix::CreateScale(0.25f) * Matrix::CreateTranslation(_position);
        // auto wvp = world * camera.view_projection();
        // auto light_direction = Vector3::TransformNormal(camera.position() - _position, world.Invert());
        // light_direction.Normalize();
        // _mesh->render(wvp, texture_storage, _colour, 1.0f, light_direction);
    }

    uint16_t CameraSink::room() const
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
    }

    CameraSink::Type CameraSink::type() const
    {
        return _type;
    }

    bool CameraSink::visible() const
    {
        return _visible;
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
