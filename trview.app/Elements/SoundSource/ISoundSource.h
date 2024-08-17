#pragma once

#include <functional>
#include <memory>

namespace trlevel
{
    struct tr_sound_source;
    union tr_x_sound_details;
}

namespace trview
{
    struct ISoundSource
    {
        using Source = std::function<std::shared_ptr<ISoundSource>(uint32_t, const trlevel::tr_sound_source&, const trlevel::tr_x_sound_details&)>;
        virtual ~ISoundSource() = 0;

        virtual uint16_t chance() const = 0;
        virtual uint16_t characteristics() const = 0;
        virtual uint16_t flags() const = 0;
        virtual uint16_t id() const = 0;
        virtual uint32_t number() const = 0;
        virtual DirectX::SimpleMath::Vector3 position() const = 0;
        virtual uint16_t sample() const = 0;
        virtual uint16_t volume() const = 0;
    };
}
