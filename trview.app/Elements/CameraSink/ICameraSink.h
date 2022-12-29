#pragma once

#include <trlevel/trtypes.h>
#include "../../Geometry/IRenderable.h"
#include "../../Geometry/PickResult.h"
#include "../../Geometry/IMesh.h"

namespace trview
{
    struct ICameraSink : public IRenderable
    {
        enum class Type
        {
            Camera,
            Sink
        };

        using Source = std::function<std::shared_ptr<ICameraSink>(uint32_t, const trlevel::tr_camera&, Type, const std::vector<uint16_t>& inferred_rooms)>;

        virtual ~ICameraSink() = 0;
        virtual uint16_t flag() const = 0;
        virtual std::vector<uint16_t> inferred_rooms() const = 0;
        virtual uint32_t number() const = 0;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;
        virtual DirectX::SimpleMath::Vector3 position() const = 0;
        virtual uint16_t room() const = 0;
        virtual Type type() const = 0;
        virtual void set_type(Type type) = 0;
    };

    uint16_t actual_room(const ICameraSink& camera_sink);
    std::string to_string(ICameraSink::Type type);
}
