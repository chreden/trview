/// @brief Defines an interface for working with cameras.

#pragma once

#include <SimpleMath.h>
#include <trview.common/Size.h>
#include "ProjectionMode.h"

namespace trview
{
    /// Interface for a camera.
    struct ICamera
    {
        /// Destructor for ICamera.
        virtual ~ICamera() = 0;

        /// Gets the direction that the camera considers to be 'forward'.
        /// @returns The forward direction.
        virtual DirectX::SimpleMath::Vector3 forward() const = 0;

        /// Get the bounding frustum for the camera. This is a left handed frustum and can be used for culling.
        /// @returns The bounding frustum.
        virtual const DirectX::BoundingFrustum& frustum() const = 0;

        /// Gets the current position of the camera in the world.
        /// @returns The position of the camera.
        virtual DirectX::SimpleMath::Vector3 position() const = 0;

        /// Gets the current projection matrix for the camera.
        /// @returns The projection matrix.
        virtual const DirectX::SimpleMath::Matrix& projection() const = 0;

        /// Gets the pitch rotation of the camera in radians.
        /// @returns The pitch rotation.
        virtual float rotation_pitch() const = 0;

        /// Gets the yaw rotation of the camera in radians.
        /// @returns The yaw rotation.
        virtual float rotation_yaw() const = 0;

        /// Rotate to the specified pitch.
        /// @param rotation The angle to rotate to.
        virtual void rotate_to_pitch(float rotation) = 0;

        /// Rotate to the specified yaw.
        /// @param rotation The angle to rotate to.
        virtual void rotate_to_yaw(float rotation) = 0;

        /// Set the projection mode of the camera.
        /// @param mode The new projection mode.
        virtual void set_projection_mode(ProjectionMode mode) = 0;

        /// Set the pitch rotation of the camera.
        /// @param rotation The rotation around the right vector in radians.
        virtual void set_rotation_pitch(float rotation) = 0;

        /// Set the yaw rotation of the camera.
        /// @param rotation The rotation around the up vector in radians.
        virtual void set_rotation_yaw(float rotation) = 0;

        /// Sets the viewport size for the camera. This will update the projection matrix.
        /// @param size The new size of the viewport.
        virtual void set_view_size(const Size& size) = 0;

        /// Gets the direction that the camera considers to be 'up'.
        /// @returns The up direction.
        virtual DirectX::SimpleMath::Vector3 up() const = 0;

        /// Update any movement.
        /// @param elapsed The time that has elapsed between updates.
        virtual void update(float elapsed) = 0;

        /// Gets the current view matrix for the camera.
        /// @returns The view matrix.
        virtual const DirectX::SimpleMath::Matrix& view() const = 0;

        /// Gets the current view projection matrix for the camera.
        /// @returns The view projection matrix.
        virtual const DirectX::SimpleMath::Matrix& view_projection() const = 0;

        /// Gets the current size of the viewport for the camera.
        /// @returns The viewport size.
        virtual const Size& view_size() const = 0;
    };
}
