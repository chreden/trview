#pragma once

#include <trview.common/Size.h>

#include <cstdint>
#include "Camera.h"

namespace trview
{
    class FreeCamera : public Camera
    {
    public:
        enum class Alignment
        {
            Camera,
            Axis
        };

        explicit FreeCamera(const Size& size);
        virtual ~FreeCamera() = default;
        void move(const DirectX::SimpleMath::Vector3& movement);

        DirectX::SimpleMath::Vector3 target() const;
        virtual DirectX::SimpleMath::Vector3 forward() const override;
        void set_rotation_yaw(float rotation) override;
        void set_rotation_pitch(float rotation) override;
        void set_position(const DirectX::SimpleMath::Vector3& position);

        // Set the camera alignment. This controls how the camera movement
        // is applied to the current position.
        // alignment: The new alignment mode.
        void set_alignment(Alignment alignment);
    private:
        Alignment _alignment{ Alignment::Axis };
    };
}