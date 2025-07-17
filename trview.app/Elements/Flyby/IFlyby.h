#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <vector>
#include <bitset>

#include <trlevel/trtypes.h>

#include <trview.common/Event.h>

#include "../../Geometry/IRenderable.h"

namespace trview
{
    struct IFlyby : public IRenderable
    {
        using Source = std::function<std::shared_ptr<IFlyby>(const std::vector<trlevel::tr4_flyby_camera>&, uint32_t)>;

        struct CameraState
        {
            enum class State
            {
                Active,
                Ended
            };

            State state{ State::Active };

            float speed{ 0 };

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
            DirectX::SimpleMath::Vector3 raw_direction;
            /// <summary>
            /// FOV if set
            /// </summary>
            float fov{ 90.0f };
            /// <summary>
            /// Roll angle in degress.
            /// </summary>
            float roll{ 0.0f };
            /// <summary>
            /// Progress through the current node spline.
            /// </summary>
            float t{ 0.0f };
            uint32_t room_id{ 0 };
            uint16_t timer{ 0 };
            std::bitset<16> flags;
        };

        virtual ~IFlyby() = 0;
        virtual std::vector<trlevel::tr4_flyby_camera> nodes() const = 0;
        virtual uint32_t number() const = 0;
        virtual [[nodiscard]] CameraState update_state(const CameraState& state, float delta) const = 0;

        Event<> on_changed;
    };
}
