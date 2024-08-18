#include "SoundSource.h"
#include <trlevel/trtypes.h>

using namespace DirectX::SimpleMath;

namespace trview
{
    ISoundSource::~ISoundSource()
    {
    }

    SoundSource::SoundSource(uint32_t number, const trlevel::tr_sound_source& source, const std::optional<trlevel::tr_x_sound_details>& details, trlevel::LevelVersion level_version)
        : _flags(source.Flags), _id(source.SoundID), _number(number), _position(source.x / trlevel::Scale, source.y / trlevel::Scale, source.z / trlevel::Scale)
    {
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

    int16_t SoundSource::id() const
    {
        return _id;
    }

    uint32_t SoundSource::number() const
    {
        return _number;
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

    uint16_t SoundSource::sample() const
    {
        return _sample;
    }

    uint16_t SoundSource::volume() const
    {
        return _volume;
    }
}
