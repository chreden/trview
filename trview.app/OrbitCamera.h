#pragma once

#include <trview.common/Size.h>

#include "Camera.h"

namespace trview
{
    class OrbitCamera : public Camera
    {
    public:
        explicit OrbitCamera(const Size& size);
        virtual ~OrbitCamera() = default;
        float zoom() const;
        DirectX::SimpleMath::Vector3 target() const;
        void set_target(const DirectX::SimpleMath::Vector3& target);
        void set_zoom(float zoom);
        void reset();
    private:
        const float default_zoom = 8.0f;

        virtual void update_vectors() override;

        // This is the orbit target.
        DirectX::SimpleMath::Vector3 _target;

        float _zoom{ default_zoom };
    };
}
