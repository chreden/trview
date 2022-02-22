#pragma once

#include <trview.common/Event.h>
#include <trview.app/Camera/CameraMode.h>
#include <trview.app/Camera/ProjectionMode.h>

namespace trview
{
    struct ICameraControls
    {
        virtual ~ICameraControls() = 0;
        /// <summary>
        /// Event raised when the camera mode has been selected by the user. The newly selected
        /// camera mode is passed as a parameter when the event is raised.
        /// </summary>
        /// <remarks>This event is not raised by the set_mode function.</remarks>
        Event<CameraMode> on_mode_selected;
        /// <summary>
        /// Event raised when the camera projection mode has been selected by the user. The newly selected
        /// projection mode is passed as a parameter when the event is raised.
        /// </summary>
        /// <remarks>This is event is not raised by the set_projection_mode function.</remarks>
        Event<ProjectionMode> on_projection_mode_selected;
        /// <summary>
        /// Event raised when the user clicks the reset button.
        /// </summary>
        Event<> on_reset;
        virtual void render() = 0;
        /// <summary>
        /// Set the current camera mode. This will not raise the on_mode_selected event.
        /// </summary>
        /// <param name="mode">The camera mode to change to.</param>
        /// <remarks>This will not raise the on_mode_selected event.</remarks>
        virtual void set_mode(CameraMode mode) = 0;
        /// <summary>
        /// Set the current camera projection mode. This will not raise the on_projection_mode_selected event
        /// </summary>
        /// <param name="mode">The projection mode to change to.</param>
        /// <remarks>This will not raise the on_projection_mode_selected event.</remarks>
        virtual void set_projection_mode(ProjectionMode mode) = 0;
    };
}
