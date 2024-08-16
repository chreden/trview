#pragma once

#include "ISoundSource.h"

namespace trview
{
    class SoundSource final : public ISoundSource
    {
    public:
        explicit SoundSource(uint32_t number);
        virtual ~SoundSource() = default;
        uint32_t number() const override;
    private:
        uint32_t _number{ 0u };
    };
}

