#pragma once

#include <trlevel/trtypes.h>
#include "../../Geometry/IRenderable.h"
#include "../../Geometry/PickResult.h"
#include "../../Geometry/IMesh.h"
#include "../ITrigger.h"

namespace trview
{
    struct ICameraSink : public IRenderable
    {
        enum class Type
        {
            Camera,
            Sink
        };

        using Source = std::function<std::shared_ptr<ICameraSink>(uint32_t, const trlevel::tr_camera&, Type, const std::vector<uint16_t>& inferred_rooms, const std::vector<std::weak_ptr<ITrigger>>& triggers)>;

        virtual ~ICameraSink() = 0;
        virtual DirectX::BoundingBox bounding_box() const = 0;
        virtual uint16_t box_index() const = 0;
        virtual uint16_t flag() const = 0;
        virtual std::vector<uint16_t> inferred_rooms() const = 0;
        virtual std::weak_ptr<ILevel> level() const = 0;
        virtual uint32_t number() const = 0;
        virtual bool persistent() const = 0;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;
        virtual DirectX::SimpleMath::Vector3 position() const = 0;
        virtual uint16_t room() const = 0;
        virtual void set_level(const std::weak_ptr<ILevel>& level) = 0;
        virtual uint16_t strength() const = 0;
        virtual Type type() const = 0;
        virtual void set_type(Type type) = 0;
        virtual std::vector<std::weak_ptr<ITrigger>> triggers() const = 0;
    };

    uint16_t actual_room(const ICameraSink& camera_sink);
    std::string to_string(ICameraSink::Type type);
}
