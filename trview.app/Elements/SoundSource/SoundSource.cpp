#include "SoundSource.h"

namespace trview
{
    ISoundSource::~ISoundSource()
    {
    }

    SoundSource::SoundSource(uint32_t number)
        : _number(number)
    {
    }

    uint32_t SoundSource::number() const
    {
        return _number;
    }
}
