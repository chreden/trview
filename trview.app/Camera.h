#pragma once

#include "ICamera.h"

namespace trview
{
    class Camera : public ICamera
    {
    public:
        explicit Camera(const Size& size);
        virtual ~Camera() = default;
        virtual float rotation_yaw() const override;
        virtual float rotation_pitch() const override;
        virtual Size view_size() const override;
        virtual const DirectX::SimpleMath::Matrix& view() const override;
        virtual const DirectX::SimpleMath::Matrix& projection() const override;
        virtual const DirectX::SimpleMath::Matrix& view_projection() const override;
        virtual void set_view_size(const Size& size) override;
        virtual void set_rotation_yaw(float rotation) override;
        virtual void set_rotation_pitch(float rotation) override;
        virtual const DirectX::BoundingFrustum& frustum() const override;
        virtual DirectX::SimpleMath::Vector3 position() const override;
        virtual DirectX::SimpleMath::Vector3 up() const override;
        virtual DirectX::SimpleMath::Vector3 forward() const override;
    protected:
        void calculate_view_matrix();
        void calculate_projection_matrix();
        void calculate_bounding_frustum();

        /// Function called when the forward, up, and position vectors need to be updated.
        /// This will also update the view matrix.
        virtual void update_vectors() = 0;

        const float default_pitch = -0.78539f;
        const float default_yaw = 0.0f;

        DirectX::SimpleMath::Vector3 _position;
        DirectX::SimpleMath::Vector3 _forward;
        DirectX::SimpleMath::Vector3 _up;
        float _rotation_yaw{ default_yaw };
        float _rotation_pitch{ default_pitch };
    private:
        Size _view_size;
        DirectX::SimpleMath::Matrix _view;
        DirectX::SimpleMath::Matrix _view_lh;
        DirectX::SimpleMath::Matrix _projection;
        DirectX::SimpleMath::Matrix _projection_lh;
        DirectX::SimpleMath::Matrix _view_projection;
        DirectX::BoundingFrustum _bounding_frustum;
    };
}
