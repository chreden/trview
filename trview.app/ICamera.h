#pragma once

#include <SimpleMath.h>
#include <trview.common/Size.h>

namespace trview
{
    struct ICamera
    {
        virtual ~ICamera() = 0;

        virtual const DirectX::SimpleMath::Matrix& view() const = 0;

        virtual const DirectX::SimpleMath::Matrix& projection() const = 0;

        virtual const DirectX::SimpleMath::Matrix& view_projection() const = 0;

        virtual DirectX::SimpleMath::Vector3 position() const = 0;

        virtual DirectX::SimpleMath::Vector3 up() const = 0;

        virtual DirectX::SimpleMath::Vector3 forward() const = 0;

        virtual float rotation_yaw() const = 0;

        virtual float rotation_pitch() const = 0;

        virtual Size view_size() const = 0;

        virtual void set_view_size(const Size& size) = 0;

        virtual void set_rotation_yaw(float rotation) = 0;

        virtual void set_rotation_pitch(float rotation) = 0;

        virtual const DirectX::BoundingFrustum& frustum() const = 0;
    };
}
