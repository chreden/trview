#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include <trlevel/trtypes.h>

#include "../../Geometry/IRenderable.h"

namespace trview
{
    struct IFlyby : public IRenderable
    {
        using Source = std::function<std::shared_ptr<IFlyby>(const std::vector<trlevel::tr4_flyby_camera>&)>;

        struct CameraState
        {
            uint32_t index{ 0u };
            DirectX::SimpleMath::Vector3 position;
            DirectX::SimpleMath::Vector3 direction{ DirectX::SimpleMath::Vector3::Backward };
            std::optional<float> fov;
            float roll{ 0.0f };
            float t{ 0.0f };
            float cat_t{ 0.0f };
        };

        virtual ~IFlyby() = 0;
        virtual CameraState state_at(float value) const = 0;
    };
}
