#pragma once

#include "ISoundSource.h"

namespace trview
{
    class SoundSource final : public ISoundSource
    {
    public:
        explicit SoundSource(uint32_t number,
            const trlevel::tr_sound_source& source,
            const trlevel::tr_x_sound_details& details);
        virtual ~SoundSource() = default;
        uint16_t chance() const override;
        uint16_t characteristics() const override;
        uint16_t flags() const override;
        uint16_t id() const override;
        uint32_t number() const override;
        DirectX::SimpleMath::Vector3 position() const override;
        uint16_t sample() const override;
        uint16_t volume() const override;
    private:
        uint16_t _chance{ 0u };
        uint16_t _characteristics{ 0u };
        uint16_t _flags{ 0u };
        uint16_t _id{ 0u };
        uint32_t _number{ 0u };
        DirectX::SimpleMath::Vector3 _position;
        uint16_t _sample{ 0u };
        uint16_t _volume{ 0u };
    };
}

