/// @brief A camera that orbits around a single point in the world.

#pragma once

#include "Camera.h"

namespace trview
{
    /// A camera that orbits around a single point in the world.
    class OrbitCamera : public Camera
    {
    public:
        /// Creates a new orbit camera.
        /// @param The viewport size.
        explicit OrbitCamera(const Size& size);

        /// Destructor for orbit camera.
        virtual ~OrbitCamera() = default;

        /// Reset the parameters of the camera.
        void reset();

        /// Set the current position that the camera will orbit around.
        /// @param target The position that the camera will orbit around.
        void set_target(const DirectX::SimpleMath::Vector3& target);

        /// Get the current orbit target.
        /// @returns The orbit target.
        const DirectX::SimpleMath::Vector3& target() const;
    protected:
        virtual void update_vectors() override;
    private:
        DirectX::SimpleMath::Vector3 _target;
    };
}
