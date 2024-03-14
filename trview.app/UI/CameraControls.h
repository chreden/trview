/// @file CameraControls.h
/// @brief UI element that contains the camera modes and settings.
/// 
/// Creates elements that the user can click to change the behaviour of the camera and switch
/// between different cameras.

#pragma once

#include "ICameraControls.h"
#include <string>

namespace trview
{
    /// <summary>
    /// The camera controls control has settings for the operation mode of the camera and other camera related settings.
    /// </summary>
    class CameraControls final : public ICameraControls
    {
    public:
        struct Names
        {
            static inline const std::string reset{ "Reset" };
            static inline const std::string mode{ "##Mode" };
            static inline const std::string projection_mode{ "##Projection" };
        };

        virtual ~CameraControls() = default;
        virtual void render() override;
        virtual void set_mode(CameraMode mode) override;
        virtual void set_projection_mode(ProjectionMode mode) override;
    private:
        /// <summary>
        /// Set the current camera mode.
        /// </summary>
        /// <param name="mode">The new camera mode.</param>
        /// <remarks>This will raise the on_mode_selected event.</remarks>
        void change_mode(CameraMode mode);
        /// <summary>
        /// Set the current camera projection mode.
        /// </summary>
        /// <param name="mode">The new camera projection mode.</param>
        /// <remarks>This will raise the on_projection_mode_selected event</remarks>
        void change_projection(ProjectionMode mode);

        CameraMode _mode{ CameraMode::Orbit };
        ProjectionMode _projection_mode{ ProjectionMode::Perspective };
    };
}
