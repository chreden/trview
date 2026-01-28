#include "Light.h"
#include "../Elements/ILevel.h"

namespace trview
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    namespace
    {
        using namespace trlevel;

        /// <summary>
        /// Light directions are backwards except for TR5 PC/Dreamcast.
        /// </summary>
        Vector3 get_direction(const Vector3& direction, PlatformAndVersion version)
        {
            auto d = direction;
            if (equals_any(version.version, LevelVersion::Tomb3, LevelVersion::Tomb4) ||
               (version.version == LevelVersion::Tomb5 && version.platform == Platform::PSX))
            {
                d = -d;
            }
            d.Normalize();
            return d;
        }
    }

    Light::Light(const std::shared_ptr<IMesh>& mesh, uint32_t number, const std::weak_ptr<IRoom>& room, const trlevel::tr_x_room_light& light, const std::weak_ptr<ILevel>& level)
        : _mesh(mesh), _number(number), _room(room), _position(light.position()), _colour(light.colour()), _type(light.type()),
        _intensity(light.intensity()), _fade(light.fade()), _direction(light.direction()), _in(light.in()), _out(light.out()),
        _rad_in(light.rad_in()), _rad_out(light.rad_out()), _length(light.length()), _cutoff(light.cutoff()), _range(light.range()),
        _radius(light.radius()), _density(light.density()), _level(level)
    {
        if (auto level_ptr = _level.lock())
        {
            _version = level_ptr->platform_and_version();
        }
    }

    uint32_t Light::number() const
    {
        return _number;
    }

    Colour Light::colour() const
    {
        return _colour;
    }

    std::weak_ptr<IRoom> Light::room() const
    {
        return _room;
    }

    trlevel::LightType Light::type() const
    {
        return _type;
    }

    void Light::render(const ICamera& camera, const Color&)
    {
        if (!_visible)
        {
            return;
        }

        auto world = Matrix::CreateScale(0.25f) * Matrix::CreateTranslation(_position);
        auto wvp = world * camera.view_projection();
        auto light_direction = Vector3::TransformNormal(camera.position() - _position, world.Invert());
        light_direction.Normalize();
        _mesh->render(wvp, _colour, 1.0f, light_direction);
    }

    void Light::render_direction(const ICamera& camera)
    {
        if (!_visible)
        {
            return;
        }

        auto world = Matrix::CreateScale(0.25f) * Matrix::CreateTranslation(_position);
        auto light_direction = Vector3::TransformNormal(camera.position() - _position, world.Invert());
        light_direction.Normalize();

        if (_type == trlevel::LightType::Spot || _type == trlevel::LightType::Sun)
        {
            const auto d = get_direction(_direction, _version);

            const int steps = 4;
            const Vector3 step = d * (1.0f / static_cast<float>(steps));
            for (int i = 1; i <= steps; ++i)
            {
                _mesh->render(
                    Matrix::CreateScale(0.05f) * 
                    Matrix::CreateTranslation(_position + step * static_cast<float>(i)) *
                    camera.view_projection(), _colour, 1.0f, light_direction);
            }
        }
    }

    void Light::get_transparent_triangles(ITransparencyBuffer&, const ICamera&, const Color&)
    {
    }
    
    bool Light::visible() const
    {
        return _visible;
    }

    void Light::set_visible(bool value)
    {
        _visible = value;
        on_changed();
    }

    Vector3 Light::position() const
    {
        return _position;
    }

    int32_t Light::intensity() const
    {
        return _intensity;
    }
    
    int32_t Light::fade() const
    {
        return _fade;
    }

    Vector3 Light::direction() const
    {
        return _direction;
    }

    std::weak_ptr<ILevel> Light::level() const
    {
        return _level;
    }

    PickResult Light::pick(const Vector3& position, const Vector3& direction) const
    {
        PickResult result;
        BoundingSphere sphere(_position, 0.125f);

        float distance = 0;
        if (sphere.Intersects(position, direction, distance))
        {
            result.distance = distance;
            result.hit = true;
            result.light = std::const_pointer_cast<ILight>(shared_from_this());
            result.position = position + direction * distance;
            result.type = PickResult::Type::Light;
        }

        return result;
    }

    float Light::in() const
    {
        return _in;
    }

    float Light::out() const
    {
        return _out;
    }

    float Light::rad_in() const
    {
        return _rad_in;
    }

    float Light::rad_out() const
    {
        return _rad_out;
    }

    float Light::range() const
    {
        return _range;
    }

    float Light::length() const
    {
        return _length;
    }

    float Light::cutoff() const
    {
        return _cutoff;
    }

    float Light::radius() const
    {
        return _radius;
    }

    float Light::density() const
    {
        return _density;
    }

    void Light::set_position(const Vector3& position)
    {
        _position = position;
    }

    trlevel::LevelVersion Light::level_version() const
    {
        return _version.version;
    }

    int32_t Light::filterable_index() const
    {
        return static_cast<int32_t>(_number);
    }
}
