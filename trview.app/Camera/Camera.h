/// @brief Defines base class for camera.

#pragma once

#include <optional>
#include <trview.common/Event.h>
#include "ICamera.h"

namespace trview
{
    /// Base class for cameras. Does most of the calculation of view and projection
    /// matrices based on the vector created by subclasses.
    class Camera : public ICamera
    {
    public:
        /// Create a new camera.
        /// @param size The viewport size.
        explicit Camera(const Size& size);
        virtual ~Camera() = default;
        virtual DirectX::SimpleMath::Vector3 forward() const override;
        virtual const DirectX::BoundingFrustum frustum() const override;
        virtual DirectX::SimpleMath::Vector3 position() const override;
        virtual DirectX::SimpleMath::Vector3 rendering_position() const override;
        virtual const DirectX::SimpleMath::Matrix projection() const override;
        virtual ProjectionMode projection_mode() const override;
        virtual float rotation_pitch() const override;
        virtual float rotation_yaw() const override;
        virtual void rotate_to_pitch(float rotation) override;
        virtual void rotate_to_yaw(float rotation) override;
        virtual void set_projection_mode(ProjectionMode mode) override;
        virtual void set_rotation_pitch(float rotation) override;
        virtual void set_rotation_yaw(float rotation) override;
        virtual void set_view_size(const Size& size) override;
        virtual void set_zoom(float zoom) override;
        virtual DirectX::SimpleMath::Vector3 up() const override;
        virtual void update(float elapsed) override;
        virtual const DirectX::SimpleMath::Matrix view() const override;
        virtual const DirectX::SimpleMath::Matrix view_projection() const override;
        virtual const Size view_size() const override;
        virtual float zoom() const override;
        virtual bool idle_rotation() const override;

        /// Event raised when the view of the camera has changed.
        Event<> on_view_changed;
    protected:
        /// Calculate the bounding frustum based on the view and projection matrices.
        void calculate_bounding_frustum();
        
        /// Calculate the projection matrix based on the viewport size.
        void calculate_projection_matrix();

        /// Calculate the view matrix based on the forward, up and rotation fields.
        void calculate_view_matrix();

        /// Function called when the forward, up, and position vectors need to be updated.
        /// Overriding functions should change the forward, up and position fields and update the view matrix.
        virtual void update_vectors() = 0;

        const float default_pitch = -0.78539f;
        const float default_yaw = 0.0f;
        const float default_zoom = 8.0f;

        DirectX::SimpleMath::Vector3 _position;
        DirectX::SimpleMath::Vector3 _forward;
        DirectX::SimpleMath::Vector3 _up;
        float _rotation_yaw{ default_yaw };
        float _rotation_pitch{ default_pitch };
        float _zoom{ default_zoom };
        ProjectionMode _projection_mode{ ProjectionMode::Perspective };
        std::optional<float> _last_rotation;
    private:
        Size _view_size;
        DirectX::SimpleMath::Matrix _view;
        DirectX::SimpleMath::Matrix _view_lh;
        DirectX::SimpleMath::Matrix _projection;
        DirectX::SimpleMath::Matrix _projection_lh;
        DirectX::SimpleMath::Matrix _view_projection;
        DirectX::BoundingFrustum _bounding_frustum;
        std::optional<float> _target_rotation_yaw;
        std::optional<float> _target_rotation_pitch;
        float _ortho_size{ 10.0f };
    };
}
