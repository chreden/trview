#pragma once

#include "ISoundSource.h"
#include <trlevel/LevelVersion.h>

namespace trview
{
    class SoundSource final : public ISoundSource
    {
    public:
        explicit SoundSource(uint32_t number,
            const trlevel::tr_sound_source& source,
            const std::optional<trlevel::tr_x_sound_details>& details,
            trlevel::LevelVersion level_version);
        virtual ~SoundSource() = default;
        uint16_t chance() const override;
        uint16_t characteristics() const override;
        uint16_t flags() const override;
        int16_t id() const override;
        uint32_t number() const override;
        uint8_t pitch() const override;
        DirectX::SimpleMath::Vector3 position() const override;
        uint8_t range() const override;
        uint16_t sample() const override;
        uint16_t volume() const override;
    private:
        uint16_t _chance{ 0u };
        uint16_t _characteristics{ 0u };
        uint16_t _flags{ 0u };
        int16_t _id{ 0 };
        uint32_t _number{ 0u };
        uint8_t _pitch{ 0u };
        DirectX::SimpleMath::Vector3 _position;
        uint8_t _range{ 0u };
        uint16_t _sample{ 0u };
        uint16_t _volume{ 0u };
    };
}

