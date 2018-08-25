/// @file CameraControls.h
/// @brief UI element that contains the camera modes and settings.
/// 
/// Creates elements that the user can click to change the behaviour of the camera and switch
/// between different cameras.

#pragma once

#include <trview.common/Event.h>
#include <trview.common/TokenStore.h>
#include "CameraMode.h"

namespace trview
{
    namespace ui
    {
        class Control;
        class Checkbox;
        class Slider;
    }

    /// The camera controls control has settings for the operation mode of the camera and
    /// other camera related settings.
    class CameraControls
    {
    public:
        /// Creates an instance of the CameraControls class.
        /// @param parent The control to which the instance will be added as a child.
        explicit CameraControls(ui::Control& parent);

        /// Event raised when the camera mode has been selected by the user. The newly selected
        /// camera mode is passed as a parameter when the event is raised.
        /// @remarks This event is not raised by the set_mode function.
        Event<CameraMode> on_mode_selected;

        /// Event raised when the camera sensitivity has been changed by the user. The new sensitivity
        /// value is passed as a parameter when the event is raised.
        /// @remarks This event is not raised by the set_sensitivity function.
        Event<float> on_sensitivity_changed;

        /// Event raised when the movement speed has been changed. The new movement speed value is passed
        /// as a parameter when the event is raised.
        /// @remarks This event is not raised by the set_movement_speed function.
        Event<float> on_movement_speed_changed;

        /// Event raised when the user clicks the reset button.
        Event<> on_reset;

        /// Set the sensitivity slider to the specified value.
        /// @param value The sensitivity value between 0 and 1.
        /// @remarks This will not raise the on_sensitivity_changed event.
        void set_sensitivity(float value);

        /// Set the movement speed slider to specified value.
        /// @param value The movement speed between 0 and 1.
        /// @remarks This will not raise the on_movement_speed_changed event.
        void set_movement_speed(float value);

        /// Set the current camera mode. This will not raise the on_mode_selected event.
        /// @param mode The camera mode to change to.
        /// @remarks This will not raise the on_mode_selected event.
        void set_mode(CameraMode mode);
    private:
        /// Set the current camera mode.
        /// @param mode The new camera mode.
        /// @remarks This will raise the on_mode_selected_event.
        void change_mode(CameraMode mode);

        ui::Checkbox* _orbit;
        ui::Checkbox* _free;
        ui::Checkbox* _axis;
        ui::Slider* _sensitivity;
        ui::Slider* _movement_speed;
        TokenStore _token_store;
    };
}
