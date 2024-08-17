#include "SoundSource.h"
#include <trlevel/trtypes.h>

using namespace DirectX::SimpleMath;

namespace trview
{
    ISoundSource::~ISoundSource()
    {
    }

    SoundSource::SoundSource(uint32_t number, const trlevel::tr_sound_source& source, const trlevel::tr_x_sound_details& details)
        : _flags(source.Flags), _id(source.SoundID), _number(number), _position(source.x / trlevel::Scale, source.y / trlevel::Scale, source.z / trlevel::Scale),
        _sample(details.tr_sound_details.Sample), _chance(details.tr_sound_details.Chance), _characteristics(details.tr_sound_details.Characteristics), _volume(details.tr_sound_details.Volume)
    {
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

    uint16_t SoundSource::id() const
    {
        return _id;
    }

    uint32_t SoundSource::number() const
    {
        return _number;
    }

    Vector3 SoundSource::position() const
    {
        return _position;
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
