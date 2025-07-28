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
        /// Alignment mode for the camera.
        enum class Alignment
        {
            /// The camera will move relative to its own axes.
            Camera,
            /// The camera will move relative to the world axes.
            Axis
        };

        /// Create a new camera.
        /// @param size The viewport size.
        explicit Camera(const Size& size);
        virtual ~Camera() = default;
        virtual DirectX::SimpleMath::Vector3 forward() const override;
        float fov() const override;
        virtual const DirectX::BoundingFrustum frustum() const override;
        virtual DirectX::SimpleMath::Vector3 position() const override;
        virtual DirectX::SimpleMath::Vector3 rendering_position() const override;
        virtual const DirectX::SimpleMath::Matrix projection() const override;
        virtual ProjectionMode projection_mode() const override;
        virtual float rotation_pitch() const override;
        float rotation_roll() const override;
        virtual float rotation_yaw() const override;
        virtual void rotate_to_pitch(float rotation) override;
        virtual void rotate_to_yaw(float rotation) override;
        virtual void set_projection_mode(ProjectionMode mode) override;
        virtual void set_rotation_pitch(float rotation) override;
        virtual void set_rotation_yaw(float rotation) override;
        virtual void set_view_size(const Size& size) override;
        virtual void set_zoom(float zoom) override;
        virtual DirectX::SimpleMath::Vector3 up() const override;
        void update(float elapsed, const DirectX::SimpleMath::Vector3& movement) override;
        virtual const DirectX::SimpleMath::Matrix view() const override;
        virtual const DirectX::SimpleMath::Matrix view_projection() const override;
        virtual const Size view_size() const override;
        virtual float zoom() const override;
        virtual bool idle_rotation() const override;
        virtual void set_fov(float value) override;
        void set_settings(const UserSettings& settings) override;
        void set_target(const DirectX::SimpleMath::Vector3& target) override;
        void set_position(const DirectX::SimpleMath::Vector3& position) override;
        void set_mode(Mode mode) override;
        void reset() override;
        DirectX::SimpleMath::Vector3 target() const override;
        Mode mode() const override;
        void set_forward(const DirectX::SimpleMath::Vector3& forward) override;
        void set_rotation_roll(float roll) override;
    private:
        /// Calculate the bounding frustum based on the view and projection matrices.
        void calculate_bounding_frustum();

        /// Calculate the projection matrix based on the viewport size.
        void calculate_projection_matrix();

        /// Calculate the view matrix based on the forward, up and rotation fields.
        void calculate_view_matrix();

        /// Function called when the forward, up, and position vectors need to be updated.
        void update_vectors();

        /// Move the camera around the world.
        /// @param movement The axis relative movement vector.
        /// @param elapsed The elapsed time since the previous update.
        /// @remarks The movement will be applied based on the current alignment mode of the camera.
        void move(const DirectX::SimpleMath::Vector3& movement, float elapsed);
        void update_acceleration(const DirectX::SimpleMath::Vector3& movement, float elapsed);
        void free_update(float elapsed, const DirectX::SimpleMath::Vector3& movement);

        const float default_pitch = -0.78539f;
        const float default_yaw = 0.0f;
        const float default_roll = 0.0f;
        const float default_zoom = 8.0f;
        static constexpr float default_fov = 45;

        DirectX::SimpleMath::Vector3 _position;
        DirectX::SimpleMath::Vector3 _forward{ DirectX::SimpleMath::Vector3::Backward };
        DirectX::SimpleMath::Vector3 _up;
        float _rotation_yaw{ default_yaw };
        float _rotation_pitch{ default_pitch };
        float _rotation_roll{ default_roll };
        float _zoom{ default_zoom };
        ProjectionMode _projection_mode{ ProjectionMode::Perspective };
        std::optional<float> _last_rotation;

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
        float _fov{ default_fov };

        Mode _mode{ Mode::Orbit };
        // Free camera settings:
        Alignment _alignment{ Alignment::Axis };
        bool _acceleration_enabled{ true };
        float _acceleration_rate{ 0.5f };
        float _acceleration{ 0.0f };
        float _movement_speed{ 0.5f };

        // Orbit camera settings
        DirectX::SimpleMath::Vector3 _target;
    };
}
