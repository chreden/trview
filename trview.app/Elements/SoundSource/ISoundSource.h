#pragma once

#include <functional>
#include <memory>
#include <optional>

#include <trlevel/LevelVersion.h>

#include "../../Geometry/IRenderable.h"
#include "../../Geometry/PickResult.h"

namespace trlevel
{
    struct tr_sound_source;
    union tr_x_sound_details;
}

namespace trview
{
    struct ISoundSource : public IRenderable
    {
        using Source = std::function<std::shared_ptr<ISoundSource>(uint32_t, const trlevel::tr_sound_source&, const std::optional<trlevel::tr_x_sound_details>&, trlevel::LevelVersion)>;
        virtual ~ISoundSource() = 0;

        virtual uint16_t chance() const = 0;
        virtual uint16_t characteristics() const = 0;
        virtual uint16_t flags() const = 0;
        virtual int16_t id() const = 0;
        virtual uint32_t number() const = 0;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;
        virtual uint8_t pitch() const = 0;
        virtual DirectX::SimpleMath::Vector3 position() const = 0;
        virtual uint8_t range() const = 0;
        virtual uint16_t sample() const = 0;
        virtual uint16_t volume() const = 0;
    };
}
