/// @brief Defines an interface for working with cameras.

#pragma once

#include <SimpleMath.h>
#include <trview.common/Size.h>
#include <trview.common/Event.h>
#include "ProjectionMode.h"

namespace trview
{
    struct UserSettings;

    /// Interface for a camera.
    struct ICamera
    {
        /// Defines the behaviour of the camera and the way it reacts to input.
        enum class Mode
        {
            /// The camera is orbiting around the centre of a room.
            Orbit,
            /// The camera is free roaming - the user is in control and can freely rotate on the spot
            /// and can move in any direction.
            Free,
            /// The camera is free roaming but movement is constrained to the axes.
            Axis
        };

        /// Destructor for ICamera.
        virtual ~ICamera() = 0;

        /// Gets the direction that the camera considers to be 'forward'.
        /// @returns The forward direction.
        virtual DirectX::SimpleMath::Vector3 forward() const = 0;

        /// Get the bounding frustum for the camera. This is a left handed frustum and can be used for culling.
        /// @returns The bounding frustum.
        virtual const DirectX::BoundingFrustum frustum() const = 0;

        /// Gets the current position of the camera in the world.
        /// @returns The position of the camera.
        virtual DirectX::SimpleMath::Vector3 position() const = 0;

        /// Gets the position that the camera is rendering from, after adjusting for projection mode for example.
        /// @returns The position from which the camera is rendering.
        virtual DirectX::SimpleMath::Vector3 rendering_position() const = 0;

        /// Gets the current projection matrix for the camera.
        /// @returns The projection matrix.
        virtual const DirectX::SimpleMath::Matrix projection() const = 0;

        /// Gets the current projection mode for the camera.
        /// @returns The projection mode.
        virtual ProjectionMode projection_mode() const = 0;

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

        /// Sets the zoom for the camera.
        /// @param zoom The zoom level.
        virtual void set_zoom(float zoom) = 0;

        /// Gets the direction that the camera considers to be 'up'.
        /// @returns The up direction.
        virtual DirectX::SimpleMath::Vector3 up() const = 0;

        /// Update any movement.
        /// @param elapsed The time that has elapsed between updates.
        virtual void update(float elapsed, const DirectX::SimpleMath::Vector3& movement) = 0;

        /// Gets the current view matrix for the camera.
        /// @returns The view matrix.
        virtual const DirectX::SimpleMath::Matrix view() const = 0;

        /// Gets the current view projection matrix for the camera.
        /// @returns The view projection matrix.
        virtual const DirectX::SimpleMath::Matrix view_projection() const = 0;

        /// Gets the current size of the viewport for the camera.
        /// @returns The viewport size.
        virtual const Size view_size() const = 0;

        /// Gets the zoom level.
        /// @returns The zoom level.
        virtual float zoom() const = 0;

        virtual bool idle_rotation() const = 0;

        virtual void set_fov(float value) = 0;

        virtual void set_settings(const UserSettings& setting) = 0;

        virtual void set_target(const DirectX::SimpleMath::Vector3& target) = 0;

        virtual void set_position(const DirectX::SimpleMath::Vector3& position) = 0;

        virtual void set_mode(Mode mode) = 0;

        virtual void reset() = 0;

        virtual DirectX::SimpleMath::Vector3 target() const = 0;

        virtual Mode mode() const = 0;

        /// Event raised when the view of the camera has changed.
        Event<> on_view_changed;
        Event<Mode> on_mode_changed;
    };
}
