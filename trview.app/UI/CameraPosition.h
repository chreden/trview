/// @file CameraPosition.h
/// @brief Displays the camera position on the screen.

#pragma once

#include <SimpleMath.h>
#include <trview.common/Event.h>

namespace trview
{
    /// Displays the camera position on the screen.
    class CameraPosition final
    {
    public:
        struct Names
        {
            static const inline std::string x { "X" };
            static const inline std::string y { "Y" };
            static const inline std::string z { "Z" };
            static const inline std::string yaw{ "Yaw" };
            static const inline std::string pitch{ "Pitch" };
        };

        void render();

        /// Update the position text.
        /// @param position The camera position.
        void set_position(const DirectX::SimpleMath::Vector3& position);

        /// <summary>
        /// Set the camera rotation.
        /// </summary>
        /// <param name="yaw">Yaw in radians.</param>
        /// <param name="pitch">Pitch in radians.</param>
        void set_rotation(float yaw, float pitch);

        /// Event raised when the user changes the camera position.
        Event<DirectX::SimpleMath::Vector3> on_position_changed;

        /// <summary>
        /// Set whether to use degrees for the camera angle display.
        /// </summary>
        /// <param name="value">True to use degrees - radians if false.</param>
        void set_display_degrees(bool value);

        Event<float, float> on_rotation_changed;
    private:
        DirectX::SimpleMath::Vector3 _position;
        float _rotation_yaw{ 0 };
        float _rotation_pitch{ 0 };
        bool _display_degrees{ true };
    };
}
