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
            /// <summary>
            /// Progress through the entire flyby.
            /// </summary>
            float value{ 0.0f };
            /// <summary>
            /// Current node index.
            /// </summary>
            int32_t index{ 0u };
            /// <summary>
            /// Current position.
            /// </summary>
            DirectX::SimpleMath::Vector3 position;
            /// <summary>
            /// Current direction.
            /// </summary>
            DirectX::SimpleMath::Vector3 direction{ DirectX::SimpleMath::Vector3::Backward };
            /// <summary>
            /// FOV if set
            /// </summary>
            std::optional<float> fov;
            /// <summary>
            /// Roll angle in degress.
            /// </summary>
            float roll{ 0.0f };
            /// <summary>
            /// Progress through the current node spline.
            /// </summary>
            float t{ 0.0f };
        };

        virtual ~IFlyby() = 0;
        virtual CameraState state_at(float value) const = 0;
        virtual [[nodiscard]] CameraState update_state(const CameraState& state, float delta) const = 0;
    };
}
