/// @file CameraPosition.h
/// @brief Displays the camera position on the screen.

#pragma once

#include <SimpleMath.h>
#include <trview.ui/Control.h>
#include <trview.ui/StackPanel.h>
#include <trview.ui/Label.h>
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
    private:
        TokenStore _token_store;
        ui::StackPanel* _display;
        ui::Label* _x;
        ui::Label* _y;
        ui::Label* _z;
    };
}
