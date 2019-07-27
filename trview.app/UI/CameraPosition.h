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

        /// Event raised when the user changes the camera position.
        Event<DirectX::SimpleMath::Vector3> on_position_changed;
    private:
        /// Attempt to update the specified coordinate by converting the text to a float.
        /// If successful this will also raise the on_position_changed event.
        void update_coordinate(float& coordinate, const std::wstring& text);

        /// Create a coordinate entry text field.
        ui::TextArea* create_coordinate_entry(ui::Control& parent, float& coordinate, const std::wstring& name);

        TokenStore _token_store;
        ui::StackPanel* _display;
        ui::TextArea* _x;
        ui::TextArea* _y;
        ui::TextArea* _z;
        DirectX::SimpleMath::Vector3 _position;
    };
}
