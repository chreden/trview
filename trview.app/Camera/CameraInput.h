#pragma once

#include <SimpleMath.h>
#include <cstdint>

#include <trview.input/Mouse.h>
#include <trview.common/Event.h>

#include <trview.app/Camera/CameraMode.h>

namespace trview
{
    /// Converts mouse and keyboard input into the control events for the camera.
    class CameraInput final
    {
    public:
        /// Get the movement vector based on the current input state.
        /// @returns The movement vector.
        DirectX::SimpleMath::Vector3 movement() const;

        /// Process a key being pressed.
        /// @param key The key that was pressed.
        /// @param control Whether the control key is pressed.
        void key_down(uint16_t key, bool control);

        /// Process a key being released.
        /// @param key The key that was released.
        void key_up(uint16_t key);

        /// Process a mouse button being pressed.
        /// @param button The button that was pressed.
        void mouse_down(input::Mouse::Button button);

        /// Process a mouse button being released.
        /// @param button The button that was released.
        void mouse_up(input::Mouse::Button button);

        /// Process the mouse being moved.
        /// @param x The x movement.
        /// @param y The y movement.
        void mouse_move(long x, long y);

        /// Process the mouse wheel being turned.
        /// @param scroll The mouse wheel movement.
        void mouse_scroll(int16_t scroll);

        /// Event raised when the camera needs to be rotated.
        Event<float, float> on_rotate;

        /// Event raised when the zoom level needs to change.
        Event<float> on_zoom;

        /// Event raised when the user pans around the scene.
        Event<float, float> on_pan;

        /// Event raised when the camera mode needs to change.
        Event<CameraMode> on_mode_change;
    private:
        bool _free_forward{ false };
        bool _free_left{ false };
        bool _free_right{ false };
        bool _free_backward{ false };
        bool _free_up{ false };
        bool _free_down{ false };
        bool _rotating{ false };
        bool _panning{ false };
    };
}