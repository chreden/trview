#include "Light.h"

namespace trview
{
    using namespace DirectX;
    using namespace DirectX::SimpleMath;

    Light::Light(const std::shared_ptr<IMesh>& mesh, uint32_t number, uint32_t room, const trlevel::tr_x_room_light& light)
        : _mesh(mesh), _number(number), _room(room), _position(light.position()), _colour(light.colour()), _type(light.type()),
        _intensity(light.intensity()), _fade(light.fade()), _direction(light.direction()), _in(light.in()), _out(light.out()),
        _rad_in(light.rad_in()), _rad_out(light.rad_out()), _length(light.length()), _cutoff(light.cutoff()), _range(light.range()),
        _radius(light.radius()), _density(light.density()), _level_version(light.level_version)
    {
    }

    uint32_t Light::number() const
    {
        return _number;
    }

    Colour Light::colour() const
    {
        return _colour;
    }

    uint32_t Light::room() const
    {
        return _room;
    }

    trlevel::LightType Light::type() const
    {
        return _type;
    }

    void Light::render(const ICamera& camera, const ILevelTextureStorage& texture_storage, const Color&)
    {
        if (!_visible)
        {
            return;
        }

        auto world = Matrix::CreateScale(0.25f) * Matrix::CreateTranslation(_position);
        auto wvp = world * camera.view_projection();
        auto light_direction = Vector3::TransformNormal(camera.position() - _position, world.Invert());
        light_direction.Normalize();
        _mesh->render(wvp, texture_storage, _colour, 1.0f, light_direction);
    }

    void Light::render_direction(const ICamera& camera, const ILevelTextureStorage& texture_storage)
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
            // Invert direction - it's stored negated in Tomb4
            auto d = _direction;
            if (_level_version == trlevel::LevelVersion::Tomb4)
            {
                d = -d;
            }
            d.Normalize();

            const int steps = 4;
            const Vector3 step = d * (1.0f / static_cast<float>(steps));
            for (int i = 1; i <= steps; ++i)
            {
                _mesh->render(
                    Matrix::CreateScale(0.05f) * 
                    Matrix::CreateTranslation(_position + step * i) *
                    camera.view_projection(), texture_storage, _colour, 1.0f, light_direction);
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

    PickResult Light::pick(const Vector3& position, const Vector3& direction) const
    {
        PickResult result;
        BoundingSphere sphere(_position, 0.125f);

        float distance = 0;
        if (sphere.Intersects(position, direction, distance))
        {
            result.distance = distance;
            result.hit = true;
            result.index = _number;
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
        return _level_version;
    }
}
