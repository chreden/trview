#include "SoundSource.h"
#include <trlevel/trtypes.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace trview
{
    ISoundSource::~ISoundSource()
    {
    }

    SoundSource::SoundSource(const std::shared_ptr<IMesh>& mesh, const std::shared_ptr<ITextureStorage>& texture_storage, uint32_t number, const trlevel::tr_sound_source& source, const std::optional<trlevel::tr_x_sound_details>& details, trlevel::LevelVersion level_version, const std::weak_ptr<ILevel>& level)
        : _mesh(mesh), _flags(source.Flags), _id(source.SoundID), _number(number), _position(source.x / trlevel::Scale, source.y / trlevel::Scale, source.z / trlevel::Scale), _level(level)
    {
        _sound_texture = texture_storage->lookup("sound_texture");

        if (!details.has_value())
        {
            return;
        }

        if (level_version < trlevel::LevelVersion::Tomb3)
        {
            _sample = details.value().tr_sound_details.Sample;
            _volume = details.value().tr_sound_details.Volume;
            _chance = details.value().tr_sound_details.Chance;
            _characteristics = details.value().tr_sound_details.Characteristics;
        }
        else
        {
            _sample = details.value().tr3_sound_details.Sample;
            _volume = details.value().tr3_sound_details.Volume;
            _range = details.value().tr3_sound_details.Range;
            _chance = details.value().tr3_sound_details.Chance;
            _pitch = details.value().tr3_sound_details.Pitch;
            _characteristics = details.value().tr3_sound_details.Characteristics;
        }
    }

    uint16_t SoundSource::chance() const
    {
        return _chance;
    }

    uint16_t SoundSource::characteristics() const
    {
        return _characteristics;
    }

    uint16_t SoundSource::flags() const
    {
        return _flags;
    }

    void SoundSource::get_transparent_triangles(ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&)
    {
    }

    int16_t SoundSource::id() const
    {
        return _id;
    }

    uint32_t SoundSource::number() const
    {
        return _number;
    }

    PickResult SoundSource::pick(const Vector3& position, const Vector3& direction) const
    {
        PickResult result{};
        BoundingBox cube(_position, Vector3(0.125f, 0.125f, 0.125f));

        float distance = 0;
        if (cube.Intersects(position, direction, distance))
        {
            result.distance = distance;
            result.hit = true;
            result.position = position + direction * distance;
            result.type = PickResult::Type::SoundSource;
            result.sound_source = std::const_pointer_cast<ISoundSource>(shared_from_this());
        }

        return result;
    }

    uint8_t SoundSource::pitch() const
    {
        return _pitch;
    }

    Vector3 SoundSource::position() const
    {
        return _position;
    }

    uint8_t SoundSource::range() const
    {
        return _range;
    }

    void SoundSource::render(const ICamera& camera, const DirectX::SimpleMath::Color&)
    {
        if (!_visible)
        {
            return;
        }

        auto world = Matrix::CreateScale(0.25f) * Matrix::CreateTranslation(_position);
        auto wvp = world * camera.view_projection();
        auto light_direction = Vector3::TransformNormal(camera.position() - _position, world.Invert());
        light_direction.Normalize();
        _mesh->render(wvp, _sound_texture, Colour::White, 1.0f, light_direction);
    }

    std::optional<int16_t> SoundSource::sample() const
    {
        return _sample;
    }

    void SoundSource::set_visible(bool value)
    {
        _visible = value;
        on_changed();
    }

    bool SoundSource::visible() const
    {
        return _visible;
    }

    uint16_t SoundSource::volume() const
    {
        return _volume;
    }

    std::weak_ptr<ILevel> SoundSource::level() const
    {
        return _level;
    }

    int32_t SoundSource::filterable_index() const
    {
        return static_cast<int32_t>(_number);
    }
}
