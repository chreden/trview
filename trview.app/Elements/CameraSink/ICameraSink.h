#pragma once

#include <trlevel/trtypes.h>
#include "../../Geometry/IRenderable.h"

namespace trview
{
    struct ICameraSink : public IRenderable
    {
        using Source = std::function<std::shared_ptr<ICameraSink>(uint32_t index, const trlevel::tr_camera& camera, uint16_t inferred_room)>;

        virtual ~ICameraSink() = 0;
        virtual uint16_t flag() const = 0;
        virtual uint16_t inferred_room() const = 0;
        virtual uint32_t number() const = 0;
        virtual DirectX::SimpleMath::Vector3 position() const = 0;
        virtual uint16_t room() const = 0;
    };
}
