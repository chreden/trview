#pragma once

#include <cstdint>
#include <external/DirectXTK/Inc/SimpleMath.h>
#include <trlevel/ILevel.h>
#include <trlevel/trtypes.h>
#include <trview.app/Geometry/IRenderable.h>
#include <trview.app/Geometry/PickResult.h>
#include <trview.app/Graphics/IMeshStorage.h>

namespace trview
{
    struct IEntity : public IRenderable
    {
        using EntitySource =
            std::function<std::shared_ptr<IEntity> (const trlevel::ILevel&, const trlevel::tr2_entity&, uint32_t, const IMeshStorage&)>;
        using AiSource =
            std::function<std::shared_ptr<IEntity>(const trlevel::ILevel&, const trlevel::tr4_ai_object&, uint32_t, const IMeshStorage&)>;

        virtual ~IEntity() = 0;
        virtual uint16_t room() const = 0;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;
        virtual DirectX::BoundingBox bounding_box() const = 0;
    };
}
