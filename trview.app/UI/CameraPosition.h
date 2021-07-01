/// @file CameraPosition.h
/// @brief Displays the camera position on the screen.

#pragma once

#include <SimpleMath.h>
#include <trview.ui/Control.h>
#include <trview.ui/StackPanel.h>
#include <trview.ui/TextArea.h>
#include <trview.common/TokenStore.h>

namespace trview
{
    /// Displays the camera position on the screen.
    class CameraPosition final
    {
    public:
        /// Create a new CameraPosition display.
        /// @param parent The parent control.
        explicit CameraPosition(ui::Control& parent);

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

        void set_display_degrees(bool value);

        Event<float, float> on_rotation_changed;
    private:
        /// Attempt to update the specified coordinate by converting the text to a float.
        /// If successful this will also raise the on_position_changed event.
        void update_position_coordinate(float& coordinate, const std::wstring& text);

        /// Attempt to update the specified coordinate by converting the text to a float.
        /// If successful this will also raise the on_rotation_changed event.
        void update_rotation_coordinate(float& coordinate, const std::wstring& text);

        /// Create a coordinate entry text field.
        ui::TextArea* create_coordinate_entry(ui::Control& parent, float& coordinate, const std::wstring& name, bool is_rotation = false);

        TokenStore _token_store;
        ui::StackPanel* _position_display;
        ui::TextArea* _x;
        ui::TextArea* _y;
        ui::TextArea* _z;
        ui::StackPanel* _rotation_display;
        ui::TextArea* _yaw;
        ui::TextArea* _pitch;
        DirectX::SimpleMath::Vector3 _position;
        float _rotation_yaw{ 0 };
        float _rotation_pitch{ 0 };
        bool _display_degrees{ true };
    };
}
