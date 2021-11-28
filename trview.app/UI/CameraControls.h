/// @file CameraControls.h
/// @brief UI element that contains the camera modes and settings.
/// 
/// Creates elements that the user can click to change the behaviour of the camera and switch
/// between different cameras.

#pragma once

#include <trview.common/TokenStore.h>
#include <trview.ui/Checkbox.h>
#include "ICameraControls.h"

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
            static const std::string reset;
            static const std::string orbit;
            static const std::string free;
            static const std::string axis;
            static const std::string ortho;
        };

        /// <summary>
        /// Creates an instance of the CameraControls class.
        /// </summary>
        /// <param name="parent">The control to which the instance will be added as a child.</param>
        /// <param name="source">The function to call to find the UI elements.</param>
        explicit CameraControls(ui::Control& parent, const ui::UiSource& source);
        virtual ~CameraControls() = default;
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

        ui::Checkbox* _orbit;
        ui::Checkbox* _free;
        ui::Checkbox* _axis;
        ui::Checkbox* _ortho;
        TokenStore _token_store;
    };
}
