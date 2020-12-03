/// @brief Defines a camera that can move freely around the world.

#pragma once

#include "Camera.h"

namespace trview
{
    /// A camera that can move freely around the world.
    class FreeCamera : public Camera
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

        /// Create a new free camera.
        /// @param size The viewport size.
        explicit FreeCamera(const Size& size);

        /// Destructor for free camera.
        virtual ~FreeCamera() = default;

        /// Move the camera around the world.
        /// @param movement The axis relative movement vector.
        /// @param elapsed The elapsed time since the previous update.
        /// @remarks The movement will be applied based on the current alignment mode of the camera.
        void move(const DirectX::SimpleMath::Vector3& movement, float elapsed);

        /// Set the camera alignment. This controls how the camera movement is applied to the current position.
        /// @param alignment The new alignment mode.
        void set_alignment(Alignment alignment);

        /// Set the position of the camera in the world.
        /// @param position The new camera position.
        void set_position(const DirectX::SimpleMath::Vector3& position);
    protected:
        virtual void update_vectors() override;
    private:
        void update_acceleration(const DirectX::SimpleMath::Vector3& movement, float elapsed);

        Alignment _alignment{ Alignment::Axis };
        float _acceleration{ 0.0f };
    };
}