/// @file CameraPosition.h
/// @brief Displays the camera position on the screen.

#pragma once

#include <SimpleMath.h>
#include <trview.ui/Control.h>
#include <trview.ui/TextArea.h>
#include <trview.common/TokenStore.h>
#include <trview.ui/ILoader.h>

namespace trview
{
    /// Displays the camera position on the screen.
    class CameraPosition final
    {
    public:
        struct Names
        {
            static const std::string x;
            static const std::string y;
            static const std::string z;
            static const std::string yaw;
            static const std::string pitch;
        };

        /// <summary>
        /// Create a new CameraPositon display.
        /// </summary>
        /// <param name="parent">The parent control.</param>
        /// <param name="ui_source">The UI source function.</param>
        explicit CameraPosition(ui::Control& parent, const ui::ILoader& ui_source);

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
        /// Attempt to update the specified coordinate by converting the text to a float.
        /// If successful this will also raise the on_position_changed event.
        void update_position_coordinate(float& coordinate, const std::wstring& text);

        /// Attempt to update the specified coordinate by converting the text to a float.
        /// If successful this will also raise the on_rotation_changed event.
        void update_rotation_coordinate(float& coordinate, const std::wstring& text);

        void bind_coordinate_entry(ui::TextArea* entry, float& coordinate, bool is_rotation = false);

        TokenStore _token_store;
        ui::TextArea* _x;
        ui::TextArea* _y;
        ui::TextArea* _z;
        ui::TextArea* _yaw;
        ui::TextArea* _pitch;
        DirectX::SimpleMath::Vector3 _position;
        float _rotation_yaw{ 0 };
        float _rotation_pitch{ 0 };
        bool _display_degrees{ true };
    };
}
