#pragma once

#include <trlevel/trtypes.h>

#include "../PickResult.h"

namespace trview
{
    struct ITransparencyBuffer;
    struct IMesh;
    struct IModel
    {
        using Source = std::function<std::shared_ptr<IModel>(const trlevel::tr_model&, const std::vector<std::shared_ptr<IMesh>>&, const std::vector<DirectX::SimpleMath::Matrix>&)>;

        virtual ~IModel() = 0;
        virtual DirectX::BoundingBox bounding_box() const = 0;
        virtual PickResult pick(const DirectX::SimpleMath::Matrix& world, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;
        virtual void render(const DirectX::SimpleMath::Matrix& world, const DirectX::SimpleMath::Matrix& view_projection, const DirectX::SimpleMath::Color& colour) = 0;
        virtual void render_transparency(const DirectX::SimpleMath::Matrix& world, ITransparencyBuffer& transparency, const DirectX::SimpleMath::Color& colour) = 0;
        virtual uint32_t type_id() const = 0;
    };
}
